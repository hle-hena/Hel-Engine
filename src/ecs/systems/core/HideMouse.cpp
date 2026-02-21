/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HideMouse.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:13:56 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/21 14:19:59                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/HideMouse.hpp"
#include "ecs/Registry.hpp"

namespace	hel::sys {

HideMouse::HideMouse(Device &device, Registry &registry,
					VkDescriptorSetLayout &setLayout)
	:	ISystem(device, registry, setLayout),
		_inputState{registry.getInputState()} {
}

void	HideMouse::update(float deltaTime) {
	
}

}
