/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PipelineMap.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/22 15:07:32 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/26 15:44:46                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/PipelineMap.hpp"
#include "api/vulkan/Device.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/assets/Shader.hpp"

#include <iostream>

namespace	hel {

PipelineMap::PipelineMap(const Config &conf)
	:	_device{*conf.device},
		_assetManager{*conf.assetManager},
		_shaderPaths{conf.shaderPaths},
		_initLayout{[](auto &, auto &){}},
		_configPipeline{[](auto &){}} {
	if (conf.initPipelineLayout)
		_initLayout = conf.initPipelineLayout;
	if (conf.configurePipeline)
		_configPipeline = conf.configurePipeline;
}

PipelineMap::~PipelineMap(void) {
	if (_layout)
		vkDestroyPipelineLayout(_device.getLogical(), _layout, nullptr);
	for (auto &it: _pipelines)
		it.second.deleteGraphicsPipeline();
}

VkPipelineLayout	PipelineMap::getLayout(std::vector<VkDescriptorSetLayout> setLayouts) {
	if (_layout)
		return (_layout);
	std::vector<VkPushConstantRange>	pushConstants{};
	_initLayout(setLayouts, pushConstants);
	VkPipelineLayoutCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount = setLayouts.size();
	createInfo.pSetLayouts = setLayouts.data();
	createInfo.pushConstantRangeCount = pushConstants.size();
	createInfo.pPushConstantRanges = pushConstants.data();
	if (vkCreatePipelineLayout(_device.getLogical(), &createInfo,
								nullptr, &_layout))
		return (nullptr);
	return (_layout);
}

bool	PipelineMap::getStageInfo(void) {
	if (!_shaderStageInfos.empty())
		return (true);
	for (auto &path: _shaderPaths) {
		auto	shader = _assetManager.get<Shader>(path);
		if (!shader) {
			std::cerr << "Couldn't find the shader \"" << shader << "\""
					<< std::endl;
			return (false);
		}
		_shaderStageInfos.push_back(shader->getStageInfo());
	}
	return (!_shaderStageInfos.empty());
}

bool	PipelineMap::bindPipeline(LayoutVec setLayouts, VkRenderPass renderPass,
								VkCommandBuffer commandBuffer) {
	auto	[it, inserted] = _pipelines.try_emplace(renderPass, _device);
	auto	&pipeline = it->second;
	if (inserted) {
		if (!getLayout(setLayouts) || !getStageInfo())
			return (true);
		PipelineConfigInfo	config{};
		Pipeline::defaultPipelineConfigInfo(config);
		_configPipeline(config);
		config.pipelineLayout = _layout;
		config.renderPass = renderPass;
		if (pipeline.createGraphicsPipeline(config, _shaderStageInfos))
			return (true);
	}

	pipeline.bind(commandBuffer);
	return (false);
}

}
