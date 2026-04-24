/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pipeline.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/13 19:39:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/03 15:20:26                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>
# include <string>
# include <vector>

namespace hel {

class	Device;

struct	PipelineConfig {
	PipelineConfig() = default;
	PipelineConfig(const PipelineConfig &) = delete;
	PipelineConfig	&operator=(const PipelineConfig&) = delete;

	std::vector<VkVertexInputAttributeDescription>		attributeDescription{};
	std::vector<VkVertexInputBindingDescription>		bindingDescription{};
	VkPipelineViewportStateCreateInfo					viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo				inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo				rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo				multisampleInfo;
	std::vector<VkPipelineColorBlendAttachmentState>	colorBlendAttachment{};
	VkPipelineDepthStencilStateCreateInfo				depthStencilInfo;
	std::vector<VkDynamicState>							dynamicStateEnables{};
	VkPipelineDynamicStateCreateInfo					dynamicStateInfo;
	VkPipelineLayout									pipelineLayout{nullptr};
	uint32_t											subpass{0};

	VkPipelineRenderingCreateInfo						renderingInfo{};
};

class	Pipeline {
	public:
		Pipeline(Device &device);
		~Pipeline(void);

		Pipeline(const Pipeline&) = delete;
		Pipeline	&operator=(const Pipeline&) = delete;

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}

		void	bind(VkCommandBuffer commandBuffer);
		bool	createGraphicsPipeline(PipelineConfig &config,
					const std::vector<VkPipelineShaderStageCreateInfo> &stageInfo);
		void	deleteGraphicsPipeline(void);

		static void	defaultPipelineconfig(PipelineConfig &config);
		static void	setBlendAttachment(PipelineConfig &config, uint32_t index,
						VkPipelineColorBlendAttachmentState attachment);
		template <typename VertexType>
		static void	setVertexInputDescriptions(PipelineConfig &config);

	private:
		bool				_healthy{true};
		std::string			_reason{""};
		Device				&_device;
		VkPipeline			_graphicsPipeline{VK_NULL_HANDLE};
};

}

# include "api/vulkan/Pipeline.tpp"
