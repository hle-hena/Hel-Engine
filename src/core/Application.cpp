/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Application.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:32 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/30 16:29:23                                        */
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
#include "ecs/Component.hpp"

#include <iostream>

namespace hel {

Application::Application(void)
	:	_appWindows{},
		_vkContext{*this},
		_registry{_assetManager},
		_assetManager{_vkContext.getDevice()},
		_engine{_vkContext.getDevice(), _registry} {
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
	if (_engine.init())
		RETURN_SET_UNHEALTHY(_engine.getReason());
	_registry.addComponent<Transform>(0);
}

Application::~Application(void) {
}

void	Application::run(void) {
	uint32_t	currentFrame = 0;

	while (!_appWindows.empty() && _healthy) {
		glfwPollEvents();

		for (size_t i = 0; i < _appWindows.size(); i++) {
			if (_appWindows[i]->shouldClose()) {
				_appWindows.erase(_appWindows.begin() + i);
				i--;
				continue ;
			}
			_engine.runFrame(*_appWindows[i], currentFrame);
		}

		currentFrame = (currentFrame + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
	}
	vkDeviceWaitIdle(_vkContext.getDevice().getLogical());
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
