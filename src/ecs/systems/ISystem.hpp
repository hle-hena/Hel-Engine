/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 14:44:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/26 15:46:22                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>

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

		virtual void	update(float) {}
		virtual void	render(VkRenderPass, WindowResources &, uint32_t) {}

	protected:
		Device					&_device;
		Registry				&_registry;
};

inline	ISystem::~ISystem(void) {}

}
