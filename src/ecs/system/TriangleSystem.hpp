/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: TriangleSystem.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 14:24:57 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/29 16:06:41                                        */
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

class	TriangleSystem {
	public:
		TriangleSystem(Device &device, Registry &registry);
		~TriangleSystem(void);

		TriangleSystem(const TriangleSystem &) = delete;
		TriangleSystem	operator=(const TriangleSystem &) = delete;

		void	update(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex);

	private:
		struct	SystemPipeline {
			SystemPipeline(TriangleSystem &system, VkFormat format);
			~SystemPipeline(void);

			bool	init(void);
			bool	createRenderPass(void);
			bool	createPipeline(void);

			VkFormat		_format;
			Pipeline		_pipeline;
			VkRenderPass	_renderPass;
			TriangleSystem	&_system;
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
