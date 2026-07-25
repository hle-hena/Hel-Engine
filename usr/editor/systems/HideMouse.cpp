/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HideMouse.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:13:56 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/25 17:31:56                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "systems/HideMouse.hpp"

namespace	hel::sys {

SystemRegistrar<HideMouse>	reg_HideMouseSystem;

void	HideMouse::init(void) {
	addUpdateDep("input/logic/hide mouse editor", &HideMouse::update);
}

void	HideMouse::update(const ExecutionContext &ctx) {
	if (!_inputState->isFocused())	{ return ; }
	if (_inputState->isPressed<input::Mouse>(GLFW_MOUSE_BUTTON_RIGHT))
		glfwSetInputMode(ctx.window->getWindow(),
			GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else if (_inputState->isReleased<input::Mouse>(GLFW_MOUSE_BUTTON_RIGHT))
		glfwSetInputMode(ctx.window->getWindow(), GLFW_CURSOR,
			GLFW_CURSOR_NORMAL);
}

}
