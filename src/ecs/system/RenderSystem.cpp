/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderSystem.cpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:14:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 13:00:38                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/system/RenderSystem.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/Device.hpp"
#include "ecs/Registry.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Assets.hpp"
#include "ecs/Component.hpp"

namespace	hel {

RenderSystem::RenderSystem(Device &device, Registry &registry)
	:	_device{device},
		_registry{registry},
		_assetManager{registry.getAssetManager()} {
}

RenderSystem::~RenderSystem(void) {
	if (_pipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(_device.getLogical(), _pipelineLayout, nullptr);
}

void	RenderSystem::update(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex) {
	auto	pipeline = getPipelineForFormat(window.getFormat());
	if (pipeline == nullptr || commandBuffer == VK_NULL_HANDLE)
		return ;

	PushConstantData	push{};
	push.viewProjection = glm::mat4{1.f};
	Entity::id	windowHandle = window.getEntityReference();
	if (windowHandle != Entity::NOT_REGISTERED) {
		push.viewProjection = _registry.getComponent<Camera>(windowHandle)->viewProjection;
	}

	beginRenderPass(commandBuffer, window, imageIndex, pipeline);

	pipeline->_pipeline.bind(commandBuffer);

	vkCmdPushConstants(commandBuffer, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
						0, sizeof(PushConstantData), &push);

	if (_tempVertexBuffer == nullptr) {
		std::vector<Vertex> vertices = {
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}}
		};

		_tempVertexBuffer = Buffer::create(_device, sizeof(vertices[0]) * vertices.size(),
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (!_tempVertexBuffer) {
			endRenderPass(commandBuffer);
			return ;
		}
		_tempVertexBuffer->writeToBuffer(static_cast<void *>(vertices.data()));
	}
	VkBuffer	buffers[] = {_tempVertexBuffer->getBuffer()};
	VkDeviceSize	offset[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	endRenderPass(commandBuffer);
}

void	RenderSystem::beginRenderPass(VkCommandBuffer commandBuffer, Window &window,
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

void	RenderSystem::endRenderPass(VkCommandBuffer commandBuffer) {
	vkCmdEndRenderPass(commandBuffer);
}



RenderSystem::SystemPipeline	*RenderSystem::getPipelineForFormat(VkFormat format) {
	if (_pipelines.find(format) != _pipelines.end())
		return (_pipelines[format].get());
	auto	pipeline = std::make_unique<SystemPipeline>(*this, format);
	if (pipeline->init())
		return (nullptr);
	_pipelines[format] = std::move(pipeline);
	return (_pipelines[format].get());
}

RenderSystem::SystemPipeline::SystemPipeline(RenderSystem &system, VkFormat format)
	:	_format{format},
		_pipeline{system._device},
		_system{system} {
}

RenderSystem::SystemPipeline::~SystemPipeline(void) {
	_pipeline.deleteGraphicsPipeline();
	if (_renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(_system._device.getLogical(), _renderPass, nullptr);
}

bool	RenderSystem::SystemPipeline::init(void) {
	return (createRenderPass() || createPipeline());
}

VkPipelineLayout	*RenderSystem::getPipelineLayout(void) {
	if (_pipelineLayout != VK_NULL_HANDLE)
		return (&_pipelineLayout);

	VkPushConstantRange	pushConstant;
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstant.size = sizeof(PushConstantData);
	pushConstant.offset = 0;

	VkPipelineLayoutCreateInfo	layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = 0;
	layoutInfo.pSetLayouts = nullptr;
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &pushConstant;

	if (vkCreatePipelineLayout(_device.getLogical(), &layoutInfo, nullptr, &_pipelineLayout))
		return (nullptr);
	return (&_pipelineLayout);
}

bool	RenderSystem::SystemPipeline::createRenderPass(void) {
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

bool	RenderSystem::SystemPipeline::createPipeline(void) {
	auto	pipelineLayout = _system.getPipelineLayout();
	if (pipelineLayout == nullptr)
		return (true);
	hel::PipelineConfigInfo	configInfo{};
	Pipeline::defaultPipelineConfigInfo(configInfo);
	Pipeline::setVertexInputDescriptions<Vertex>(configInfo);


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
