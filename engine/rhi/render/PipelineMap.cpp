/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PipelineMap.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/22 15:07:32 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 10:39:55                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/render/PipelineMap.hpp"
#include "rhi/render/Shader.hpp"
#include "rhi/context/Device.hpp"
#include "utils/mathUtils.hpp"

#include "core/ecs/AssetManager.hpp"//remove aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

#include <iostream>

namespace	hel {

size_t	RenderingConfigHasher::operator()(const RenderingConfig &config) const {
	size_t	seed = 0;
	for (const auto &colorFormat: config.colorFormats)
		hel::mathUtils::hashCombine(seed, colorFormat);
	hel::mathUtils::hashCombine(seed,
		config.depthFormat.value_or(VK_FORMAT_MAX_ENUM));
	hel::mathUtils::hashCombine(seed,
		config.stencilFormat.value_or(VK_FORMAT_MAX_ENUM));
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
	createInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
	createInfo.pSetLayouts = setLayouts.data();
	createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
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
			std::cerr << "Couldn't find the shader \"" << path << "\""
					<< std::endl;
			return (false);
		}
		_shaderStageInfos.push_back(shader->getStageInfo());
	}
	return (!_shaderStageInfos.empty());
}

bool	PipelineMap::bindPipeline(const RenderingConfig &renderingConfig,
								VkCommandBuffer commandBuffer) {
	auto	[it, inserted] = _pipelines.try_emplace(renderingConfig);
	auto	&pipeline = it->second;
	if (inserted) {
		if (!getLayout() || !getStageInfo())
			return (true);
		PipelineConfig	config{};
		Pipeline::defaultPipelineconfig(config);
		for (uint32_t i = 0; i < renderingConfig.colorFormats.size(); i++) {
			VkPipelineColorBlendAttachmentState	attachment{};
			attachment.colorWriteMask = (i != 0) ? 0 :
											VK_COLOR_COMPONENT_R_BIT |
											VK_COLOR_COMPONENT_G_BIT |
											VK_COLOR_COMPONENT_B_BIT |
											VK_COLOR_COMPONENT_A_BIT;
			attachment.blendEnable = VK_FALSE;
			Pipeline::setBlendAttachment(config, i, attachment);
		}
		_configPipeline(config);
		config.pipelineLayout = _layout;

		config.renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		config.renderingInfo.colorAttachmentCount = static_cast<uint32_t>(renderingConfig.colorFormats.size());
		config.renderingInfo.pColorAttachmentFormats = renderingConfig.colorFormats.data();
		if (renderingConfig.depthFormat.has_value())
			config.renderingInfo.depthAttachmentFormat
				= renderingConfig.depthFormat.value();
		if (renderingConfig.stencilFormat.has_value())
			config.renderingInfo.stencilAttachmentFormat
				= renderingConfig.stencilFormat.value();

		if (auto res = pipeline.init(&_device, config, _shaderStageInfos); !res) {
			std::cerr << res.error() << std::endl;
			return true;
		}
	}

	pipeline.bind(commandBuffer);
	return (false);
}

}
