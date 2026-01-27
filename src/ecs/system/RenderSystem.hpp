/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderSystem.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:14:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/27 18:46:05                                        */
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

class	RenderSystem {
	public:
		RenderSystem(Device &device, Registry &registry);

		void	update(VkCommandBuffer commandBuffer, Window &window);

	private:
		struct	SystemPipeline {
			SystemPipeline(RenderSystem &system);

			bool	init(void);
			bool	createRenderPass(void);

			Pipeline		_pipeline;
			VkRenderPass	_renderPass;
			RenderSystem	&_system;
		};
		using pipelineMap = std::unordered_map<VkFormat, std::unique_ptr<SystemPipeline>>;

		SystemPipeline	*getPipelineForFormat(VkFormat format);

		bool			_healthy{true};
		std::string		_reason{""};
		Device			&_device;
		Registry		&_registry;
		pipelineMap		_pipelines;
};

}
