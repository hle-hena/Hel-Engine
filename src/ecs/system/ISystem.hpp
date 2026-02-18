/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 14:44:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 11:18:26                                        */
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

		virtual void	update(float deltaTime) {}
		virtual void	render(WindowResources &resources,
							uint32_t currentFrame, uint32_t imageIndex) {}

	protected:
		Device					&_device;
		Registry				&_registry;
		VkDescriptorSetLayout	&_setLayout;
};

inline	ISystem::~ISystem(void) {}

}
