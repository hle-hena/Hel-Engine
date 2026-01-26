/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: DefNotTriangleSystem.cpp                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 16:58:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/26 16:59:56                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/system/DefNotTriangleSystem.hpp"
#include "api/vulkan/Device.hpp"
#include "platform/window/Window.hpp"
#include "utils/healthHelper.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"

#include <stdexcept>
#include <cassert>
#include <array>
#include <iostream>

namespace hel {

DefNotTriangleSystemPipeline::DefNotTriangleSystemPipeline(Device& device, AssetManager &assetManager, std::string vertShaderPath,
										std::string fragShaderPath, const VkFormat &format)
	:	_vertShaderPath{vertShaderPath},
		_fragShaderPath{fragShaderPath},
		_device{device},
		_format{format},
		_pipeline{device, assetManager} {
}

DefNotTriangleSystemPipeline::~DefNotTriangleSystemPipeline(void) {
	_pipeline.deleteGraphicsPipeline();
	if (_pipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(_device.getLogical(), _pipelineLayout, nullptr);
	if (_renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(_device.getLogical(), _renderPass, nullptr);
}

bool	DefNotTriangleSystemPipeline::init(void) {
	return (createRenderPass() || createPipelineLayout() || createGraphicsPipeline());
}

bool	DefNotTriangleSystemPipeline::createRenderPass(void) {
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

	if (vkCreateRenderPass(_device.getLogical(), &renderPassInfo, nullptr,
							&_renderPass) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't create a render pass", true);
	return (false);
}

bool	DefNotTriangleSystemPipeline::createPipelineLayout(void) {
	VkPipelineLayoutCreateInfo	pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(_device.getLogical(), &pipelineLayoutInfo,
								nullptr, &_pipelineLayout) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Failed to create the pipeline layout", true);
	return (false);
}

bool	DefNotTriangleSystemPipeline::createGraphicsPipeline(void) {
	if (_pipelineLayout == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Cannot create a graphics pipeline before the pipeline layout", true);
	if (_renderPass == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Cannot create a graphics pipeline without a render pass", true);

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = _renderPass;
	pipelineConfig.pipelineLayout = _pipelineLayout;

	if (_pipeline.createGraphicsPipeline(pipelineConfig, {_vertShaderPath, _fragShaderPath}))
		RETURN_SET_UNHEALTHY(_pipeline.getReason(), true);
	return (false);
}

void	DefNotTriangleSystemPipeline::bind(VkCommandBuffer commandBuffer) {
	_pipeline.bind(commandBuffer);
}



DefNotTriangleSystem::DefNotTriangleSystem(Device &device, Registry &registry, std::string vertShaderPath, std::string fragShaderPath)
	:	_vertShaderPath{vertShaderPath},
		_fragShaderPath{fragShaderPath},
		_device{device},
		_registry{registry} {
}

DefNotTriangleSystem::~DefNotTriangleSystem(void) {
}

void	DefNotTriangleSystem::update(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex) {
	DefNotTriangleSystemPipeline	*pipeline = getPipelineForFormat(window.getFormat());
	if (pipeline == nullptr || commandBuffer == VK_NULL_HANDLE)
		return ;
	beginRenderPass(commandBuffer, pipeline, window, imageIndex);

	auto	entities = _registry.view<Name, Transform>();
	for (auto entity: entities) {
		auto	&name = entities.get<Name>(entity);
		std::cout << "\rTriangle " << name.name;
		pipeline->bind(commandBuffer);
		vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	}
	endRenderPass(commandBuffer);
}

void	DefNotTriangleSystem::beginRenderPass(VkCommandBuffer commandBuffer, DefNotTriangleSystemPipeline *pipeline,
									Window &window, uint32_t imageIndex) {
	Swapchain	&swapchain = window.getSwapchain();
	VkExtent2D	extent = swapchain.getExtent();

	VkRenderPassBeginInfo	renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = pipeline->_renderPass;
	renderPassInfo.framebuffer = swapchain.getFrameBuffer(imageIndex, pipeline->_renderPass);
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = extent;
	VkClearValue	clearColor = {{{0., 0., 0., 1.}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport	viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D	scissor{};
	scissor.offset = {0, 0};
	scissor.extent = extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void	DefNotTriangleSystem::endRenderPass(VkCommandBuffer commandBuffer) {
	vkCmdEndRenderPass(commandBuffer);
}

DefNotTriangleSystemPipeline	*DefNotTriangleSystem::getPipelineForFormat(VkFormat format) {
	auto		it = _pipelines.find(format);
	if (it != _pipelines.end())
		return (it->second.get());
	auto	pipeline = std::make_unique<DefNotTriangleSystemPipeline>(_device, _registry.getAssetManager(), _vertShaderPath, _fragShaderPath, format);
	if (pipeline->init()) {
		std::cerr << "Failed to create a new NOT triangle system pipeline for the following reason:\n"
			<< pipeline->getReason() << std::endl;
		return (nullptr);
	}
	std::cout << "Created a new pipeline for the NOT triangle system" << std::endl;
	DefNotTriangleSystemPipeline	*ptr = pipeline.get();
	_pipelines[format] = std::move(pipeline);
	return (ptr);
}

}
