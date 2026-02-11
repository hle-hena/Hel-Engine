/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderSystem.cpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:14:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/11 14:58:59                                        */
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
#include "api/vulkan/Buffer.hpp"
#include "ecs/Registry.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Component.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/assets/Shader.hpp"

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
	auto	pipeline = getPipelineForFormat(window.getFormat(), window.getDepthFormat());
	if (pipeline == nullptr || commandBuffer == VK_NULL_HANDLE)
		return ;

	PushConstantData	push{};
	push.viewProjection = glm::mat4{0.f};
	Entity::id	windowHandle = window.getEntityReference();
	if (windowHandle != Entity::NOT_REGISTERED) {
		push.viewProjection = _registry.getComponent<Camera>(windowHandle)->viewProjection;
	}

	beginRenderPass(commandBuffer, window, imageIndex, pipeline);

	pipeline->_pipeline.bind(commandBuffer);

	vkCmdPushConstants(commandBuffer, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
						0, sizeof(PushConstantData), &push);

	// auto	mesh = _assetManager.get<Geometry>("assets/models/colored_cube.obj");
	auto	mesh = _assetManager.get<Geometry>("assets/models/dragon.obj");
	VkBuffer	buffers[] = {mesh->vertexBuffer->getBuffer()};
	VkDeviceSize	offset[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);
	vkCmdBindIndexBuffer(commandBuffer, mesh->indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, mesh->vertexCount, 1, 0, 0, 0);

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
	std::array<VkClearValue, 2>	clearValues{};
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};
	renderPassBegin.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBegin.pClearValues = clearValues.data();
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



RenderSystem::SystemPipeline	*RenderSystem::getPipelineForFormat(VkFormat format, VkFormat depthFormat) {
	if (_pipelines.find(format) != _pipelines.end())
		return (_pipelines[format].get());
	auto	pipeline = std::make_unique<SystemPipeline>(*this, format, depthFormat);
	if (pipeline->init())
		return (nullptr);
	_pipelines[format] = std::move(pipeline);
	return (_pipelines[format].get());
}

RenderSystem::SystemPipeline::SystemPipeline(RenderSystem &system, VkFormat format, VkFormat depthFormat)
	:	_format{format},
		_depthFormat{depthFormat},
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
	VkAttachmentDescription	depthAttachment{};
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.format = _depthFormat;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	VkAttachmentReference	colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference	depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	VkSubpassDescription	subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency	dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
							VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
							VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
								VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2>	attachments = {colorAttachment, depthAttachment};
	VkRenderPassCreateInfo	renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

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
