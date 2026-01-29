/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: TriangleSystem.cpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/28 19:01:02 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/29 11:53:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/system/TriangleSystem.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/Device.hpp"
#include "ecs/Registry.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Assets.hpp"

namespace	hel {

TriangleSystem::TriangleSystem(Device &device, Registry &registry)
	:	_device{device},
		_registry{registry},
		_assetManager{registry.getAssetManager()} {
}

TriangleSystem::~TriangleSystem(void) {
	if (_pipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(_device.getLogical(), _pipelineLayout, nullptr);
}

void	TriangleSystem::update(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex) {
	auto	pipeline = getPipelineForFormat(window.getFormat());
	if (pipeline == nullptr || commandBuffer == VK_NULL_HANDLE)
		return ;

	beginRenderPass(commandBuffer, window, imageIndex, pipeline);

	pipeline->_pipeline.bind(commandBuffer);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	endRenderPass(commandBuffer);
}

void	TriangleSystem::beginRenderPass(VkCommandBuffer commandBuffer, Window &window,
									uint32_t imageIndex, SystemPipeline *pipeline) {
	Swapchain	&swapchain = window.getSwapchain();
	VkExtent2D	extent = swapchain.getExtent();

	VkRenderPassBeginInfo	renderPassBegin{};
	renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBegin.renderPass = pipeline->_renderPass;
	renderPassBegin.framebuffer = swapchain.getFrameBuffer(imageIndex, pipeline->_renderPass);
	renderPassBegin.renderArea.extent = extent;
	renderPassBegin.renderArea.offset = {0, 0};
	VkClearValue	clearColor{{0., 0., 0., 1.}};
	renderPassBegin.clearValueCount = 1;
	renderPassBegin.pClearValues = &clearColor;
	vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport	viewport{};
	viewport.height = static_cast<float>(extent.height);
	viewport.width = static_cast<float>(extent.width);
	viewport.maxDepth = 1.f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D	scissor{};
	scissor.extent = extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void	TriangleSystem::endRenderPass(VkCommandBuffer commandBuffer) {
	vkCmdEndRenderPass(commandBuffer);
}



TriangleSystem::SystemPipeline	*TriangleSystem::getPipelineForFormat(VkFormat format) {
	if (_pipelines.find(format) != _pipelines.end())
		return (_pipelines[format].get());
	auto	pipeline = std::make_unique<SystemPipeline>(*this, format);
	if (pipeline->init())
		return (nullptr);
	_pipelines[format] = std::move(pipeline);
	return (_pipelines[format].get());
}

TriangleSystem::SystemPipeline::SystemPipeline(TriangleSystem &system, VkFormat format)
	:	_format{format},
		_pipeline{system._device},
		_system{system} {
}

TriangleSystem::SystemPipeline::~SystemPipeline(void) {
	_pipeline.deleteGraphicsPipeline();
	if (_renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(_system._device.getLogical(), _renderPass, nullptr);
}

bool	TriangleSystem::SystemPipeline::init(void) {
	return (createRenderPass() || createPipeline());
}

VkPipelineLayout	*TriangleSystem::getPipelineLayout(void) {
	if (_pipelineLayout != VK_NULL_HANDLE)
		return (&_pipelineLayout);

	VkPipelineLayoutCreateInfo	layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = 0;
	layoutInfo.pSetLayouts = nullptr;
	layoutInfo.pushConstantRangeCount = 0;
	layoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(_device.getLogical(), &layoutInfo, nullptr, &_pipelineLayout))
		return (nullptr);
	return (&_pipelineLayout);
}

bool	TriangleSystem::SystemPipeline::createRenderPass(void) {
	VkAttachmentDescription	colorAttachment{};
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.format = _format;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkAttachmentReference	colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkSubpassDescription	subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo	renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(_system._device.getLogical(), &renderPassInfo,
							nullptr, &_renderPass))
		return (true);
	return (false);
}

bool	TriangleSystem::SystemPipeline::createPipeline(void) {
	auto	pipelineLayout = _system.getPipelineLayout();
	if (pipelineLayout == nullptr)
		return (true);
	hel::PipelineConfigInfo	configInfo{};
	Pipeline::defaultPipelineConfigInfo(configInfo);

	configInfo.renderPass = _renderPass;
	configInfo.pipelineLayout = *pipelineLayout;

	auto	vert = _system._assetManager.get<Shader>(_system._vertPath);
	auto	frag = _system._assetManager.get<Shader>(_system._fragPath);
	if (!vert || !frag)
		return (true);
	if (_pipeline.createGraphicsPipeline(configInfo, {vert->getStageInfo(), frag->getStageInfo()}))
		return (true);
	return (false);
}

}
