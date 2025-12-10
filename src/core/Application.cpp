/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Application.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:32 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/10 18:06:21                                        */
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
	:	_helWindows{} {
	Window::windowPtr window = Window::createWindow(800, 600, "Hel Engine", *this);
	if (!window) {
		_available = false;
		return ;
	}
	_helWindows.push_back(std::move(window));
}

Application::~Application(void) {
}

void	Application::run(void) {
	while (_helWindows.size() > 0 && _available) {
		glfwPollEvents();

		size_t	windowCount = _helWindows.size();
		for (size_t i = 0; i < windowCount; i++) {
			if (_helWindows[i]->shouldClose()) {
				_helWindows.erase(_helWindows.begin() + i);
				i--;
				windowCount--;
			}
		}
	}
}

void	Application::addNewWindow(int width, int height, const std::string &windowName) {
	Window::windowPtr window = Window::createWindow(width, height, windowName, *this);
	if (!window) {
		return ;
	}
	_helWindows.push_back(std::move(window));
}

}
