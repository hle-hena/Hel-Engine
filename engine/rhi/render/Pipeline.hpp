/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pipeline.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/13 19:39:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 10:38:39                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "utils/Expected.hpp"

namespace hel {

class	Device;

struct	PipelineConfig {
	PipelineConfig() = default;
	PipelineConfig(const PipelineConfig &) = delete;
	PipelineConfig	&operator=(const PipelineConfig&) = delete;

	std::vector<VkVertexInputAttributeDescription>		attributeDescription{};
	std::vector<VkVertexInputBindingDescription>		bindingDescription{};
	VkPipelineViewportStateCreateInfo					viewportInfo{};
	VkPipelineInputAssemblyStateCreateInfo				inputAssemblyInfo{};
	VkPipelineRasterizationStateCreateInfo				rasterizationInfo{};
	VkPipelineMultisampleStateCreateInfo				multisampleInfo{};
	std::vector<VkPipelineColorBlendAttachmentState>	colorBlendAttachment{};
	VkPipelineDepthStencilStateCreateInfo				depthStencilInfo{};
	std::vector<VkDynamicState>							dynamicStateEnables{};
	VkPipelineDynamicStateCreateInfo					dynamicStateInfo{};
	VkPipelineLayout									pipelineLayout{nullptr};
	uint32_t											subpass{0};

	enum	Type {
		Graphics,
		Compute
	};
	Type	type{Graphics};

	VkPipelineRenderingCreateInfo						renderingInfo{};
};

class	Pipeline {
	public:
		Pipeline(void) = default;
		~Pipeline(void);

		expected<void>	init(Device *device, PipelineConfig &config,
				const std::vector<VkPipelineShaderStageCreateInfo> &stageInfo);

		Pipeline(const Pipeline&) = delete;
		Pipeline	&operator=(const Pipeline&) = delete;

		void	bind(VkCommandBuffer commandBuffer);

		static void	defaultPipelineconfig(PipelineConfig &config);
		static void	setBlendAttachment(PipelineConfig &config, uint32_t index,
						VkPipelineColorBlendAttachmentState attachment);
		template <typename VertexType>
		static void	setVertexInputDescriptions(PipelineConfig &config);

	private:
		Device				*_device;
		bool				_fullyInit;
		VkPipeline			_graphicsPipeline{VK_NULL_HANDLE};

		void			deleteGraphicsPipeline(void);
		expected<void>	createGraphicsPipeline(PipelineConfig &config,
			const std::vector<VkPipelineShaderStageCreateInfo> &stageInfo);
};

}

#include "rhi/render/Pipeline.tpp"
