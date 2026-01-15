/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: MeshSystem.cpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/13 19:30:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 16:17:26                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/system/MeshSystem.hpp"
#include "render/vulkan/Device.hpp"
#include "platform/window/Window.hpp"
#include "utils/healthHelper.hpp"

#include <stdexcept>
#include <cassert>
#include <array>
#include <iostream>

namespace hel {

MeshSystemPipeline::MeshSystemPipeline(Device& device, std::string vertShaderPath,
									std::string fragShaderPath, const VkFormat &format)
	:	_vertShaderPath{vertShaderPath},
		_fragShaderPath{fragShaderPath},
		_device{device},
		_format{format},
		_pipeline{device} {
}

MeshSystemPipeline::~MeshSystemPipeline(void) {
	_pipeline.deleteGraphicsPipeline();
	if (_pipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(_device.getLogical(), _pipelineLayout, nullptr);
	if (_renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(_device.getLogical(), _renderPass, nullptr);
}

bool	MeshSystemPipeline::init(void) {
	return (createRenderPass() || createPipelineLayout() || createGraphicsPipeline());
}

bool	MeshSystemPipeline::createRenderPass(void) {
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

bool	MeshSystemPipeline::createPipelineLayout(void) {
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

bool	MeshSystemPipeline::createGraphicsPipeline(void) {
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

void	MeshSystemPipeline::bind(VkCommandBuffer commandBuffer) {
	_pipeline.bind(commandBuffer);
}



MeshSystem::MeshSystem(Device& device, std::string vertShaderPath, std::string fragShaderPath)
	:	_vertShaderPath{vertShaderPath},
		_fragShaderPath{fragShaderPath},
		_device{device} {
}

MeshSystem::~MeshSystem(void) {
}

void	MeshSystem::render(VkCommandBuffer commandBuffer, Window &window) {
	MeshSystemPipeline	*pipeline = getPipelineForFormat(window.getFormat());
	if (pipeline == nullptr)
		return ;
	if (commandBuffer == VK_NULL_HANDLE)
		return ;
	pipeline->bind(commandBuffer);
	// vkCmdDraw(commandBuffer, 3, 1, 0, 0); // Example draw call
}

MeshSystemPipeline	*MeshSystem::getPipelineForFormat(VkFormat format) {
	auto		it = _pipelines.find(format);
	if (it != _pipelines.end())
		return (it->second.get());
	auto	pipeline = std::make_unique<MeshSystemPipeline>(_device, _vertShaderPath, _fragShaderPath, format);
	if (pipeline->init()) {
		std::cerr << "Failed to create a new mesh system pipeline for the following reason:\n"
			<< pipeline->getReason() << std::endl;
		return (nullptr);
	}
	std::cout << "Created a new pipeline for the mesh System" << std::endl;
	MeshSystemPipeline	*ptr = pipeline.get();
	_pipelines[format] = std::move(pipeline);
	return (ptr);
}

}
