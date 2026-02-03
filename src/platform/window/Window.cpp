/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 12:20:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/03 11:58:50                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/window/Window.hpp"
#include "platform/window/GLFW.hpp"
#include "core/Application.hpp"
#include "ecs/Component.hpp"

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
		if (window->_swapchain.initiateSwapChain(*window)) {
			return (nullptr);
		}
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
		_swapchain{app.getVkContext().getDevice()},
		_app{app},
		_instance{instance} {
	initWindow();
}

void	Window::initWindow(void) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	_windowPtr = glfwCreateWindow(_width, _height, _windowName.c_str(),
								nullptr, nullptr);
	glfwSetWindowUserPointer(_windowPtr, this);
	glfwSetKeyCallback(_windowPtr, keyCallback);
	glfwSetFramebufferSizeCallback(_windowPtr, frameBufferResizedCallback);
	glfwSetWindowFocusCallback(_windowPtr, focusCallback);
	glfwSetCursorPosCallback(_windowPtr, cursorPositionCallback);
	glfwSetCursorEnterCallback(_windowPtr, cursorEnterCallback);
}

Window::~Window(void) {
	deleteWindow();
}

void	Window::deleteWindow(void) {
	_swapchain.deleteSwapChain();
	if (_surface != VK_NULL_HANDLE)
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
	glfwDestroyWindow(_windowPtr);
	GLFW::release();
}

bool	Window::shouldClose(void) {
	auto	&inputState = _app.getRegistry().getInputState();

	if (inputState.isKeyReleased(GLFW_KEY_ESCAPE) &&
		inputState.getFocused() == this) {
		glfwSetWindowShouldClose(_windowPtr, true);
		inputState.setFocus(this, false);
		return (true);
	}
	if (inputState.isKeyHeld(GLFW_KEY_ESCAPE) &&
		inputState.hasMod(GLFW_MOD_CONTROL) &&
		inputState.getFocused() == this) {
		glfwSetWindowShouldClose(_windowPtr, true);
		inputState.setFocus(this, false);
		return (true);
	}
	if (inputState.isKeyPressed(GLFW_KEY_N) &&
		inputState.hasMod(GLFW_MOD_CONTROL) &&
		inputState.getFocused() == this) {
		_app.addNewWindow(Window::WIDTH, Window::HEIGHT, _windowName + "_copy");
		return (false);
	}
	return (glfwWindowShouldClose(_windowPtr));
}

void	Window::frameBufferResizedCallback(GLFWwindow *window,
												int width, int height) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	appWindow->getSwapchain()._frameBufferResized = true;

	Entity::id	handle = appWindow->getEntityReference();
	if (auto camera = appWindow->getApp().getRegistry().modify<Camera>(handle)) {
		camera->aspect = static_cast<float>(width) / static_cast<float>(height);
	}
}

void	Window::focusCallback(GLFWwindow *window, int focused) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	appWindow->getApp().getRegistry().getInputState().setFocus(appWindow, focused);
}

void	Window::keyCallback(GLFWwindow *window, int key, int scancode,
							int action, int mod) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	appWindow->getApp().getRegistry().getInputState().setKeyState(key, action, mod);
}

void	Window::cursorEnterCallback(GLFWwindow *window, int enter) {
	if (enter) {
		auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

		appWindow->_lastMouseX = -1;
	}
}

void	Window::cursorPositionCallback(GLFWwindow *window, double x, double y) {
	auto		appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	auto		&input = appWindow->getApp().getRegistry().getInputState();

	if (input.getFocused() != appWindow) { return ; }
	if (appWindow->_lastMouseX == -1) {
		appWindow->_lastMouseX = x;
		appWindow->_lastMouseY = y;
	}
	input.setMouseMove(appWindow, x - appWindow->_lastMouseX,
					y - appWindow->_lastMouseY);
	appWindow->_lastMouseX = x;
	appWindow->_lastMouseY = y;
}

}
