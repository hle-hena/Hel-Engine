/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PipelineMap.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/22 14:54:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/26 15:44:23                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <unordered_map>
# include <functional>

# include "api/vulkan/Pipeline.hpp"

namespace	hel {

class	AssetManager;

class	PipelineMap {
	public:
		using LayoutVec = std::vector<VkDescriptorSetLayout>;
		using LayoutCallback = std::function<void (
								std::vector<VkDescriptorSetLayout> &,
								std::vector<VkPushConstantRange> &)>;
		using ConfigCallback = std::function<void (PipelineConfigInfo &)>;
		struct	Config {
			Device						*device;
			AssetManager				*assetManager;
			std::vector<std::string>	shaderPaths{};

			LayoutCallback	initPipelineLayout;
			ConfigCallback	configurePipeline;
		};

		PipelineMap(const Config &config);
		~PipelineMap(void);
		PipelineMap(const PipelineMap &other) = delete;
		PipelineMap	&operator=(const PipelineMap &other) = delete;

		bool				bindPipeline(LayoutVec setLayouts,
										VkRenderPass renderPass,
										VkCommandBuffer commandBuffer);
		VkPipelineLayout	getLayout(LayoutVec setLayouts);

	private:
		bool				getStageInfo(void);

		Device											&_device;
		AssetManager									&_assetManager;
		std::vector<std::string>						_shaderPaths;
		std::vector<VkPipelineShaderStageCreateInfo>	_shaderStageInfos;
		VkPipelineLayout								_layout{VK_NULL_HANDLE};
		std::unordered_map<VkRenderPass, Pipeline>		_pipelines;

		LayoutCallback									_initLayout;
		ConfigCallback									_configPipeline;
};

}
