/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HideMouse.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:13:56 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/28 18:16:16                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/HideMouse.hpp"
#include "ecs/Registry.hpp"
#include "platform/window/Window.hpp"

namespace	hel::sys {

void	HideMouse::init(void) {
	_inputState = &_registry->getInputState();
}

void	HideMouse::update(const FrameContext &) {
	auto	window = _inputState->getFocused();
	if (!window)
		return ;
	if (_inputState->isPressed<input::Mouse>(GLFW_MOUSE_BUTTON_RIGHT))
		glfwSetInputMode(window->getWindow(),
			GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else if (_inputState->isReleased<input::Mouse>(GLFW_MOUSE_BUTTON_RIGHT))
		glfwSetInputMode(window->getWindow(), GLFW_CURSOR,
			GLFW_CURSOR_NORMAL);
}

}
