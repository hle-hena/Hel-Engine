/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pipeline.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/13 19:39:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 15:05:48                                        */
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

struct	PipelineConfigInfo {
	PipelineConfigInfo() = default;
	PipelineConfigInfo(const PipelineConfigInfo &) = delete;
	PipelineConfigInfo	&operator=(const PipelineConfigInfo&) = delete;

	VkPipelineViewportStateCreateInfo		viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo	inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo	rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo	multisampleInfo;
	VkPipelineColorBlendAttachmentState		colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo		colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo	depthStencilInfo;
	std::vector<VkDynamicState>				dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo		dynamicStateInfo;
	VkPipelineLayout						pipelineLayout = nullptr;
	VkRenderPass							renderPass = nullptr;
	uint32_t								subpass = 0;
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
		bool	createGraphicsPipeline(const std::string &vertPath,
									const std::string &fragPath,
									const PipelineConfigInfo &configInfo);
		void	deleteGraphicsPipeline(void);

		static void	defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:
		std::vector<char>	readFile(const std::string& filepath);

		bool	createShaderModule(const std::vector<char>& code,
								VkShaderModule* shaderModule);

		bool			_healthy{true};
		std::string		_reason{""};
		Device&			_device;
		VkPipeline		_graphicsPipeline{VK_NULL_HANDLE};
		VkShaderModule	_vertShaderModule{VK_NULL_HANDLE};
		VkShaderModule	_fragShaderModule{VK_NULL_HANDLE};
};

}
