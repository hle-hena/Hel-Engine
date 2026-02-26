/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 14:44:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/26 18:38:38                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "api/vulkan/PipelineMap.hpp"

# include <vulkan/vulkan.h>
# include <vector>
# include <memory>

namespace	hel {

class	Device;
class	Registry;
struct	WindowResources;

}

namespace	hel::sys {

class	ISystem {
	public:
		ISystem(Device &device, Registry &registry):
			_device{device},
			_registry{registry} {}
		virtual ~ISystem(void) = 0;

		ISystem(const ISystem &other) = delete;
		ISystem	&operator=(const ISystem &other) = delete;

		virtual void	initAllPipelines(WindowResources &initResources) final;

		virtual void	update(float) {}
		virtual void	render(VkRenderPass, WindowResources &, uint32_t) {}

	protected:
		virtual PipelineMap	*createPipeline(const PipelineMap::Config &config) final;

		Device										&_device;
		Registry									&_registry;
		std::vector<std::unique_ptr<PipelineMap>>	_pipelines;
};

inline	ISystem::~ISystem(void) {}

}
