/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: TriangleSystem.cpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/22 15:39:35                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/system/TriangleSystem.hpp"
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

TriangleSystemPipeline::TriangleSystemPipeline(Device& device, std::string vertShaderPath,
										std::string fragShaderPath, const VkFormat &format)
	:	_vertShaderPath{vertShaderPath},
		_fragShaderPath{fragShaderPath},
		_device{device},
		_format{format},
		_pipeline{device} {
}

TriangleSystemPipeline::~TriangleSystemPipeline(void) {
	_pipeline.deleteGraphicsPipeline();
	if (_pipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(_device.getLogical(), _pipelineLayout, nullptr);
	if (_renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(_device.getLogical(), _renderPass, nullptr);
}

bool	TriangleSystemPipeline::init(void) {
	return (createRenderPass() || createPipelineLayout() || createGraphicsPipeline());
}

bool	TriangleSystemPipeline::createRenderPass(void) {
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

bool	TriangleSystemPipeline::createPipelineLayout(void) {
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

bool	TriangleSystemPipeline::createGraphicsPipeline(void) {
	if (_pipelineLayout == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Cannot create a graphics pipeline before the pipeline layout", true);
	if (_renderPass == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Cannot create a graphics pipeline without a render pass", true);

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = _renderPass;
	pipelineConfig.pipelineLayout = _pipelineLayout;

	if (_pipeline.createGraphicsPipeline(_vertShaderPath, _fragShaderPath,
										pipelineConfig))
		RETURN_SET_UNHEALTHY(_pipeline.getReason(), true);
	return (false);
}

void	TriangleSystemPipeline::bind(VkCommandBuffer commandBuffer) {
	_pipeline.bind(commandBuffer);
}



TriangleSystem::TriangleSystem(Device &device, Registry &registry, std::string vertShaderPath, std::string fragShaderPath)
	:	_vertShaderPath{vertShaderPath},
		_fragShaderPath{fragShaderPath},
		_device{device},
		_registry{registry} {
}

TriangleSystem::~TriangleSystem(void) {
}

void	TriangleSystem::update(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex) {
	TriangleSystemPipeline	*pipeline = getPipelineForFormat(window.getFormat());
	if (pipeline == nullptr || commandBuffer == VK_NULL_HANDLE)
		return ;
	beginRenderPass(commandBuffer, pipeline, window, imageIndex);

	Pool<Name>	&pool = _registry.getPool<Name>();
	int i = 0;
	for (auto &entity: pool.components) {
		pipeline->bind(commandBuffer);
		vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	}

	endRenderPass(commandBuffer);
}

void	TriangleSystem::beginRenderPass(VkCommandBuffer commandBuffer, TriangleSystemPipeline *pipeline,
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

void	TriangleSystem::endRenderPass(VkCommandBuffer commandBuffer) {
	vkCmdEndRenderPass(commandBuffer);
}

TriangleSystemPipeline	*TriangleSystem::getPipelineForFormat(VkFormat format) {
	auto		it = _pipelines.find(format);
	if (it != _pipelines.end())
		return (it->second.get());
	auto	pipeline = std::make_unique<TriangleSystemPipeline>(_device, _vertShaderPath, _fragShaderPath, format);
	if (pipeline->init()) {
		std::cerr << "Failed to create a new mesh system pipeline for the following reason:\n"
			<< pipeline->getReason() << std::endl;
		return (nullptr);
	}
	std::cout << "Created a new pipeline for the mesh System" << std::endl;
	TriangleSystemPipeline	*ptr = pipeline.get();
	_pipelines[format] = std::move(pipeline);
	return (ptr);
}

}
