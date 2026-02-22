/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PipelineMap.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/22 14:54:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/22 15:33:56                                        */
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

# include "api/vulkan/Pipeline.hpp"
# include "ecs/systems/ISystem.hpp"

namespace	hel {

class	PipelineMap {
	public:
		PipelineMap(Device &device, VkDescriptorSetLayout &globalSetLayout,
					sys::ISystem &system);
		~PipelineMap(void);
		PipelineMap(const PipelineMap &other) = delete;
		PipelineMap	&operator=(const PipelineMap &other) = delete;

		bool	bindPipeline(VkRenderPass renderPass,
							VkCommandBuffer commandBuffer);

	private:
		VkPipelineLayout	getLayout(void);

		Device											&_device;
		sys::ISystem									&_system;
		VkDescriptorSetLayout							&_globalSetLayout;
		std::vector<VkPipelineShaderStageCreateInfo>	_shaderStageInfos;
		VkPipelineLayout								_layout{VK_NULL_HANDLE};
		std::unordered_map<VkRenderPass, Pipeline>		_pipelines;
};

}
