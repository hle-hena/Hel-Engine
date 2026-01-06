/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 12:20:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/06 16:25:39                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/window/Window.hpp"
#include "platform/window/GLFW.hpp"
#include "platform/events/KeyEvent.hpp"
#include "core/Application.hpp"

namespace	hel {

Window::windowPtr	Window::createWindow(int width, int height,
										const std::string &windowName,
										Application &app, VkInstance &instance) noexcept {
	if (!GLFW::acquire())
		return (nullptr);
	try {
		Window::windowPtr	window = Window::windowPtr(new Window(width, height,
																windowName, app, instance));
		if (glfwCreateWindowSurface(instance, window->getWindow(),
									nullptr, &window->_surface) != VK_SUCCESS)
			return (nullptr);
		if (window->_swapChain.initiateSwapChain(*window))
			return (nullptr);
		return (window);
	} catch (...) {
		GLFW::release();
		return (nullptr);
	}
}

Window::windowPtr	Window::createBootstrap(int width, int height,
										const std::string &windowName,
										Application &app, VkInstance &instance) noexcept {
	if (!GLFW::acquire())
		return (nullptr);
	try {
		Window::windowPtr	window = Window::windowPtr(new Window(width, height,
																windowName, app, instance));
		if (glfwCreateWindowSurface(instance, window->getWindow(),
									nullptr, &window->_surface) != VK_SUCCESS)
			return (nullptr);
		return (window);
	} catch (...) {
		GLFW::release();
		return (nullptr);
	}
}

Window::Window(int width, int height, const std::string &windowName,
			Application &app, VkInstance &instance)
	:	_width(width),
		_height(height),
		_windowName(windowName),
		_windowPtr(nullptr),
		_swapChain{app.getVkContext().getDevice()},
		_app{app},
		_instance{instance} {
	initWindow();
}

void	Window::initWindow(void) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	_windowPtr = glfwCreateWindow(_width, _height, _windowName.c_str(),
								nullptr, nullptr);
	glfwSetWindowUserPointer(_windowPtr, this);
	glfwSetKeyCallback(_windowPtr, keyEventCallback);
	// glfwSetFramebufferSizeCallback(_windowPtr, frameBufferResizedCallback);
}

Window::~Window(void) {
	deleteWindow();
}

void	Window::deleteWindow(void) {
	_swapChain.deleteSwapChain();
	if (_surface != VK_NULL_HANDLE)
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
	glfwDestroyWindow(_windowPtr);
	GLFW::release();
}

// void	Window::frameBufferResizedCallback(GLFWwindow *window,
// 												int width, int height) {
// 	auto _rtWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
// 	_rtWindow->_frameBufferResized = true;
// 	_rtWindow->_width = width;
// 	_rtWindow->_height = height;
// }

}
