/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 12:20:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/10 16:10:47                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/window/Window.hpp"
#include "platform/window/GLFW.hpp"

#include <stdexcept>

namespace	hel {

Window::windowPtr	Window::createWindow(int width, int height,
										const std::string &windowName) noexcept {
	if (!GLFW::acquire())
		return (nullptr);
	try {
		return (Window::windowPtr(new Window(width, height, windowName)));
	} catch (...) {
		GLFW::release();
		return (nullptr);
	}
}

Window::Window(int width, int height,
							const std::string &windowName)
	:	_width(width),
		_height(height),
		_windowName(windowName),
		_windowPtr(nullptr) {
	initWindow();
}

void	Window::initWindow(void) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	_windowPtr = glfwCreateWindow(_width, _height, _windowName.c_str(),
								nullptr, nullptr);
	glfwSetWindowUserPointer(_windowPtr, this);
	// glfwSetFramebufferSizeCallback(_windowPtr, frameBufferResizedCallback);
}

Window::~Window(void) {
	deleteWindow();
}

void	Window::deleteWindow(void) {
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

// void	Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
// {
// 	if (glfwCreateWindowSurface(instance, _windowPtr, nullptr, surface) != VK_SUCCESS)
// 		throw std::runtime_error("Failed to create a window surface.");
// }

}
