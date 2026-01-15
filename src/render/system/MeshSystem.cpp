/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: MeshSystem.cpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/13 19:30:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 12:25:38                                        */
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

namespace hel {

MeshSystem::MeshSystem(Device& device, Window &window)
	:	_device{device},
		_window{window},
		_pipeline{device} {
}

MeshSystem::~MeshSystem() {
	if (_pipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(_device.getLogical(), _pipelineLayout, nullptr);
	if (_renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(_device.getLogical(), _renderPass, nullptr);
}

bool	MeshSystem::initMeshSystem() {
	return (createRenderPass() || createPipelineLayout() || createGraphicsPipeline());
}

bool	MeshSystem::createRenderPass(void) {
	VkAttachmentDescription	colorAttachment{};
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.format = _window.getFormat();
	//This assumes that every window after the first one will have the same format.
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

bool	MeshSystem::createPipelineLayout() {
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

bool	MeshSystem::createGraphicsPipeline() {
	if (_pipelineLayout == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Cannot create a graphics pipeline before the pipeline layout", true);
	if (_renderPass == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Cannot create a graphics pipeline without a render pass", true);

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = _renderPass;
	pipelineConfig.pipelineLayout = _pipelineLayout;

	if (_pipeline.createGraphicsPipeline("shaders/simple_shader.vert.spv",
										"shaders/simple_shader.frag.spv",
										pipelineConfig))
		RETURN_SET_UNHEALTHY(_pipeline.getReason(), true);
	return (false);
}

void MeshSystem::render(VkCommandBuffer commandBuffer) {
	_pipeline.bind(commandBuffer);
	// vkCmdDraw(commandBuffer, 3, 1, 0, 0); // Example draw call
}

}
