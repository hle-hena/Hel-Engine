/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PipelineMap.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/22 15:07:32 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/24 18:02:44                                        */
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
#include "utils/mathUtils.hpp"

#include <iostream>

namespace	hel {

size_t	RenderingConfigHasher::operator()(const RenderingConfig &config) const {
	size_t	seed = 0;
	for (const auto &colorFormat: config.colorFormats)
		hel::mathUtils::hashCombine(seed, colorFormat);
	hel::mathUtils::hashCombine(seed, config.depthFormat);
	return (seed);
}

bool	RenderingConfig::operator==(const RenderingConfig &other) const {
	if (colorFormats.size() != other.colorFormats.size())
		return (false);
	if (depthFormat != other.depthFormat)
		return (false);
	return (colorFormats == other.colorFormats);
}

PipelineMap::PipelineMap(const Config &conf)
	:	_device{*conf.device},
		_assetManager{*conf.assetManager},
		_shaderPaths{conf.shaderPaths},
		_initLayout{[](auto &, auto &, auto &){}},
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

void	PipelineMap::initDefaultSets(std::vector<VkDescriptorSetLayout> sets) {
	_defaultLayouts = sets;
}

VkPipelineLayout	PipelineMap::getLayout(void) {
	if (_layout)
		return (_layout);
	std::vector<VkDescriptorSetLayout>	setLayouts = _defaultLayouts;
	std::vector<VkPushConstantRange>	pushConstants{};
	_initLayout(_device, setLayouts, pushConstants);
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

bool	PipelineMap::bindPipeline(const RenderingConfig &renderingConfig,
								VkCommandBuffer commandBuffer) {
	auto	[it, inserted] = _pipelines.try_emplace(renderingConfig, _device);
	auto	&pipeline = it->second;
	if (inserted) {
		if (!getLayout() || !getStageInfo())
			return (true);
		PipelineConfigInfo	config{};
		Pipeline::defaultPipelineConfigInfo(config);
		_configPipeline(config);
		config.pipelineLayout = _layout;

		config.renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		config.renderingInfo.colorAttachmentCount = renderingConfig.colorFormats.size();
		config.renderingInfo.pColorAttachmentFormats = renderingConfig.colorFormats.data();
		config.renderingInfo.depthAttachmentFormat = renderingConfig.depthFormat;

		if (pipeline.createGraphicsPipeline(config, _shaderStageInfos))
			return (true);
	}

	pipeline.bind(commandBuffer);
	return (false);
}

}
