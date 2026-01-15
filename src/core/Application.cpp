/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Application.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:32 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 18:55:11                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Application.hpp"
#include "utils/healthHelper.hpp"
#include "platform/window/GLFW.hpp"

#include <iostream>

namespace hel {

Application::Application(void)
	:	_appWindows{},
		_vkContext{*this},
		_renderer{_vkContext.getDevice()} {
	if (!GLFW::acquire())
		RETURN_SET_UNHEALTHY("Couldn't init glfw.");
	if (_vkContext.initiateVulkan()) {
		GLFW::release();
		RETURN_SET_UNHEALTHY(_vkContext.getReason());
	}
	addNewWindow(Window::WIDTH, Window::HEIGHT, "Hel");
	GLFW::release();
	if (_appWindows.size() == 0)
		RETURN_SET_UNHEALTHY("Couldn't even create one window");
	if (_renderer.init())
		RETURN_SET_UNHEALTHY(_renderer.getReason());
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
				continue ;
			}
			_renderer.drawFrame(*_appWindows[i]);
		}
	}
	if (!_healthy)
		std::cerr << _reason << std::endl;
}

void	Application::addNewWindow(int width, int height, const std::string &windowName) {
	Window::windowPtr window = Window::createWindow(width, height, windowName, *this,
												_vkContext.getInstance().getVkInstance());
	if (!window) {
		std::cerr << "Failed to create a new window." << std::endl;
		return ;
	}
	if (!_vkContext.getDevice().supportSurface(*window.get())) {
		std::cerr << "The window surface is not supported." << std::endl;
		return ;
	}
	_appWindows.push_back(std::move(window));
}

}
