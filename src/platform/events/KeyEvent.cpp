/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: KeyEvent.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 17:37:36 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/10 18:07:09                                        */
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

void	keyEventCallback(GLFWwindow *window, int key, int scanCode,
						int action, int mod) {
	Window		*helWindow = reinterpret_cast<Window *>
								(glfwGetWindowUserPointer(window));
	Application	&hel = helWindow->getApp();

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (key == GLFW_KEY_N && action == GLFW_PRESS
			&& mod == GLFW_MOD_CONTROL) {
		VkExtent2D	extent = helWindow->getExtent();
		hel.addNewWindow(extent.width, extent.height, helWindow->getWindowName() + "_copy");
	}
}

}
