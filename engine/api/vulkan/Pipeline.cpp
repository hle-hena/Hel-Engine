/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pipeline.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/13 19:39:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 15:55:24                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Pipeline.hpp"
#include "api/vulkan/Device.hpp"

namespace hel {

Pipeline::~Pipeline() {
	deleteGraphicsPipeline();
}

expected<void>	Pipeline::init(Device *device, PipelineConfig &config,
			const std::vector<VkPipelineShaderStageCreateInfo> &stageInfo) {
	_device = device;
	if (config.type == PipelineConfig::Compute)
		return unexpected("Not yet supported pipeline: Compute pipeline.");
	return createGraphicsPipeline(config, stageInfo);
}

void	Pipeline::deleteGraphicsPipeline(void) {
	if (_graphicsPipeline != VK_NULL_HANDLE)
		vkDestroyPipeline(_device->getLogical(), _graphicsPipeline, nullptr);
}

void Pipeline::bind(VkCommandBuffer commandBuffer) {
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
}

expected<void>	Pipeline::createGraphicsPipeline(PipelineConfig &config,
			const std::vector<VkPipelineShaderStageCreateInfo> &stageInfo) {
	if (config.pipelineLayout == VK_NULL_HANDLE)
		return unexpected("Missing pipeline layout for pipeline creation");

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pVertexAttributeDescriptions = config.attributeDescription.data();
	vertexInputInfo.pVertexBindingDescriptions = config.bindingDescription.data();
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(config.attributeDescription.size());
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(config.bindingDescription.size());

	VkPipelineColorBlendStateCreateInfo	blendStateInfo{};
	blendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendStateInfo.logicOpEnable = VK_FALSE;
	blendStateInfo.attachmentCount = static_cast<uint32_t>(config.colorBlendAttachment.size());
	blendStateInfo.pAttachments = config.colorBlendAttachment.data();

	VkGraphicsPipelineCreateInfo	pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = &config.renderingInfo;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = static_cast<uint32_t>(stageInfo.size());
	pipelineInfo.pStages = stageInfo.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pViewportState = &config.viewportInfo;
	pipelineInfo.pRasterizationState = &config.rasterizationInfo;
	pipelineInfo.pMultisampleState = &config.multisampleInfo;
	pipelineInfo.pDepthStencilState = &config.depthStencilInfo;
	pipelineInfo.pColorBlendState = &blendStateInfo;
	pipelineInfo.pDynamicState = &config.dynamicStateInfo;
	pipelineInfo.layout = config.pipelineLayout;
	pipelineInfo.renderPass = VK_NULL_HANDLE;
	pipelineInfo.subpass = config.subpass;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;
	if (vkCreateGraphicsPipelines(_device->getLogical(), VK_NULL_HANDLE, 1,
								&pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS)
		return unexpected("Failed to create a pipeline");
	return {};
}

void	Pipeline::setBlendAttachment(PipelineConfig &config, uint32_t index,
							VkPipelineColorBlendAttachmentState attachment) {
	if (config.colorBlendAttachment.size() <= index) {
		VkPipelineColorBlendAttachmentState	defaultVal{};
		defaultVal.colorWriteMask = 0;
		defaultVal.blendEnable = VK_FALSE;
		config.colorBlendAttachment.resize(index + 1, defaultVal);
	}
	config.colorBlendAttachment[index] = attachment;
}

void Pipeline::defaultPipelineconfig(PipelineConfig &config) {
	config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	config.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	config.viewportInfo.viewportCount = 1;
	config.viewportInfo.pViewports = nullptr;
	config.viewportInfo.scissorCount = 1;
	config.viewportInfo.pScissors = nullptr;

	config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	config.rasterizationInfo.depthClampEnable = VK_FALSE;
	config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	config.rasterizationInfo.lineWidth = 1.0f;
	config.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
	config.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	config.rasterizationInfo.depthBiasEnable = VK_FALSE;

	config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	config.multisampleInfo.sampleShadingEnable = VK_FALSE;
	config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	config.depthStencilInfo.depthTestEnable = VK_TRUE;
	config.depthStencilInfo.depthWriteEnable = VK_TRUE;
	config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	config.depthStencilInfo.stencilTestEnable = VK_FALSE;

	config.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	config.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	config.dynamicStateInfo.pDynamicStates = config.dynamicStateEnables.data();
	config.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.dynamicStateEnables.size());
	config.dynamicStateInfo.flags = 0;
}

}
