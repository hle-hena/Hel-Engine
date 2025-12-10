/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Application.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:32 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/10 16:28:13                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Application.hpp"

namespace hel {

Application::Application(void)
	:	_helWindow{nullptr} {
	Window::windowPtr window = Window::createWindow(800, 600, "Hel Engine");
	if (!window)
	{
		_unavailable = true;
		return ;
	}
	_helWindow = std::move(window);
}

Application::~Application(void) {
}

void	Application::run(void) {
	while (!_helWindow->shouldClose())
	{
		glfwPollEvents();
	}
}

}
