/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: KeyEvent.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 17:37:36 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/19 17:07:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/events/KeyEvent.hpp"
#include "platform/window/Window.hpp"
#include "core/Application.hpp"

namespace	hel {

void	keyEventCallback(GLFWwindow *glfwWindow, int key, int scanCode,
						int action, int mod) {
	Window		*window = reinterpret_cast<Window *>
								(glfwGetWindowUserPointer(glfwWindow));
	Application	&app = window->getApp();

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(glfwWindow, true);
	else if (key == GLFW_KEY_N && action == GLFW_PRESS
			&& mod == GLFW_MOD_CONTROL) {
		app.addNewWindow(Window::WIDTH, Window::HEIGHT, window->getWindowName() + "_copy");
	}
}

}
