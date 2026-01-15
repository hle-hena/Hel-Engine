/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pipeline.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/13 19:39:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 15:19:39                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/vulkan/Pipeline.hpp"
#include "render/vulkan/Device.hpp"
#include "utils/healthHelper.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace hel {

Pipeline::Pipeline(Device& device)
	: _device{device} {
}

Pipeline::~Pipeline() {
}

void	Pipeline::deleteGraphicsPipeline(void) {
	if (_vertShaderModule != VK_NULL_HANDLE)
		vkDestroyShaderModule(_device.getLogical(), _vertShaderModule, nullptr);
	if (_fragShaderModule != VK_NULL_HANDLE)
		vkDestroyShaderModule(_device.getLogical(), _fragShaderModule, nullptr);
	if (_graphicsPipeline != VK_NULL_HANDLE)
		vkDestroyPipeline(_device.getLogical(), _graphicsPipeline, nullptr);
}

void Pipeline::bind(VkCommandBuffer commandBuffer) {
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
}

std::vector<char> Pipeline::readFile(const std::string& filepath) {
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		return (std::vector<char>(0));

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

bool	Pipeline::createGraphicsPipeline(const std::string &vertPath,
										const std::string &fragPath,
										const PipelineConfigInfo &configInfo) {
	if (configInfo.pipelineLayout == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Missing pipeline layout for pipeline creation", true);
	if (configInfo.renderPass == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Missing render pass for pipeline creation", true);

	auto	vertCode = readFile(vertPath);
	if (vertCode.size() == 0)
		RETURN_SET_UNHEALTHY("Failed to open file: " + vertPath, true);
	auto	fragCode = readFile(fragPath);
	if (fragCode.size() == 0)
		RETURN_SET_UNHEALTHY("Failed to open file: " + fragPath, true);
	if (createShaderModule(vertCode, &_vertShaderModule) ||
		createShaderModule(fragCode, &_fragShaderModule))
		return (true);

	VkPipelineShaderStageCreateInfo	stageInfo[2];
	stageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo[0].pNext = nullptr;
	stageInfo[0].flags = 0;
	stageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stageInfo[0].module = _vertShaderModule;
	stageInfo[0].pName = "main";
	stageInfo[0].pSpecializationInfo = nullptr;
	stageInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo[1].pNext = nullptr;
	stageInfo[1].flags = 0;
	stageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stageInfo[1].module = _fragShaderModule;
	stageInfo[1].pName = "main";
	stageInfo[1].pSpecializationInfo = nullptr;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	// TODO: Add vertex binding descriptions and attribute descriptions here
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.vertexBindingDescriptionCount = 0;

	VkGraphicsPipelineCreateInfo	pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = stageInfo;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pViewportState = &configInfo.viewportInfo;
	pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
	pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
	pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
	pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
	pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
	pipelineInfo.layout = configInfo.pipelineLayout;
	pipelineInfo.renderPass = configInfo.renderPass;
	pipelineInfo.subpass = configInfo.subpass;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;
	//TODO -> cache the pipeline I think ?
	if (vkCreateGraphicsPipelines(_device.getLogical(), VK_NULL_HANDLE, 1,
								&pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Failed to create a pipeline", true);
	return (false);
}

bool	Pipeline::createShaderModule(const std::vector<char>& code,
									VkShaderModule* shaderModule) {
	VkShaderModuleCreateInfo	shaderModuleInfo;
	shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.pNext = nullptr;
    shaderModuleInfo.flags = 0;
    shaderModuleInfo.codeSize = code.size();
    shaderModuleInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

	if (vkCreateShaderModule(_device.getLogical(), &shaderModuleInfo, nullptr,
							shaderModule) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Failed to create a shader module", true);
	return (false);
}

void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo& configInfo) {
	configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	configInfo.viewportInfo.viewportCount = 1;
	configInfo.viewportInfo.pViewports = nullptr;
	configInfo.viewportInfo.scissorCount = 1;
	configInfo.viewportInfo.pScissors = nullptr;

	configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
	configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	configInfo.rasterizationInfo.lineWidth = 1.0f;
	configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

	configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
	configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	configInfo.colorBlendAttachment.blendEnable = VK_FALSE;

	configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
	configInfo.colorBlendInfo.attachmentCount = 1;
	configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;

	configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
	configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
	configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;

	configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
	configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
	configInfo.dynamicStateInfo.flags = 0;
}

}
