/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderSystem.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:14:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/28 18:42:02                                        */
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
# include <memory>
# include <string>

# include "api/vulkan/Pipeline.hpp"

namespace	hel {

class	Window;
class	Device;
class	Registry;
class	AssetManager;

class	RenderSystem {
	public:
		RenderSystem(Device &device, Registry &registry);
		~RenderSystem(void);

		void	update(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex);

	private:
		struct	SystemPipeline {
			SystemPipeline(RenderSystem &system, VkFormat format);
			~SystemPipeline(void);

			bool	init(void);
			bool	createRenderPass(void);
			bool	createPipeline(void);

			VkFormat		_format;
			Pipeline		_pipeline;
			VkRenderPass	_renderPass;
			RenderSystem	&_system;
		};
		using pipelineMap = std::unordered_map<VkFormat, std::unique_ptr<SystemPipeline>>;

		SystemPipeline		*getPipelineForFormat(VkFormat format);
		VkPipelineLayout	*getPipelineLayout(void);

		void				beginRenderPass(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex, SystemPipeline *pipeline);
		void				endRenderPass(VkCommandBuffer commandBuffer);

		bool				_healthy{true};
		std::string			_reason{""};
		std::string			_vertPath{"assets/shaders/triangle.vert.spv"};
		std::string			_fragPath{"assets/shaders/triangle.frag.spv"};
		Device				&_device;
		Registry			&_registry;
		AssetManager		&_assetManager;
		pipelineMap			_pipelines;
		VkPipelineLayout	_pipelineLayout{VK_NULL_HANDLE};
};

}
