/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 14:44:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/22 16:52:44                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <vector>

namespace	hel {

class	Device;
class	Registry;
struct	WindowResources;

}

namespace	hel::sys {

class	ISystem {
	public:
		ISystem(Device &device, Registry &registry,
				VkDescriptorSetLayout &setLayout):
			_device{device},
			_registry{registry},
			_setLayout{setLayout} {}
		virtual ~ISystem(void) = 0;

		ISystem(const ISystem &other) = delete;
		ISystem	&operator=(const ISystem &other) = delete;

		virtual void	update(float) {}
		virtual void	render(VkRenderPass, WindowResources &, uint32_t) {}

	protected:
		Device					&_device;
		Registry				&_registry;
		VkDescriptorSetLayout	&_setLayout;
};

inline	ISystem::~ISystem(void) {}

}
