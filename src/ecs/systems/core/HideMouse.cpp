/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HideMouse.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:13:56 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/21 15:32:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/HideMouse.hpp"
#include "ecs/Registry.hpp"
#include "platform/window/Window.hpp"

namespace	hel::sys {

HideMouse::HideMouse(Device &device, Registry &registry,
					VkDescriptorSetLayout &setLayout)
	:	ISystem(device, registry, setLayout),
		_inputState{registry.getInputState()} {
}

void	HideMouse::update(float deltaTime) {
	if (_inputState.isPressed<input::Mouse>(GLFW_MOUSE_BUTTON_RIGHT))
		glfwSetInputMode(_inputState.getFocused()->getWindow(),
			GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else if (_inputState.isReleased<input::Mouse>(GLFW_MOUSE_BUTTON_RIGHT))
		glfwSetInputMode(_inputState.getFocused()->getWindow(), GLFW_CURSOR,
			GLFW_CURSOR_NORMAL);
}

}
