/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Application.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:32 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/12 15:41:24                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Application.hpp"

#include <iostream>

namespace hel {

Application::Application(void)
	:	_appWindows{} {
	if (!_appDevice.isHealthy()) {
		_healthy = false;
		std::cerr << _appDevice.getReason() << std::endl;
		return ;
	}
	addNewWindow(Window::WIDTH, Window::HEIGHT, "Hel Engine");
	if (_appWindows.size() == 0)
		_healthy = false;
}

Application::~Application(void) {
}

void	Application::run(void) {
	while (_appWindows.size() > 0 && _healthy) {
		glfwPollEvents();

		size_t	windowsCount = _appWindows.size();
		for (size_t i = 0; i < windowsCount; i++) {
			if (_appWindows[i]->shouldClose()) {
				_appWindows.erase(_appWindows.begin() + i);
				i--;
				windowsCount--;
			}
		}
	}
}

void	Application::addNewWindow(int width, int height, const std::string &windowName) {
	Window::windowPtr window = Window::createWindow(width, height, windowName, *this);
	if (!window) {
		std::cerr << "Failed to create a new window." << std::endl;
		return ;
	}
	_appWindows.push_back(std::move(window));
}

}
