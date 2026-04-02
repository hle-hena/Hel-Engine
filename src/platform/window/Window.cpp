/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 12:20:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 17:28:04                                        */
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

Window::windowPtr	Window::createWindow(uint32_t width, uint32_t height,
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

Window::windowPtr	Window::createBootstrap(uint32_t width, uint32_t height,
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

Window::Window(uint32_t width, uint32_t height, const std::string &windowName,
			Application &app, VkInstance &instance)
	:	_app{app},
		_instance{instance},
		_width(width),
		_height(height),
		_uiContext{this},
		_windowName(windowName),
		_windowPtr(nullptr),
		_swapchain{app.getVkContext().getDevice()} {
	initWindow();
}

void	Window::initWindow(void) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	_windowPtr = glfwCreateWindow(_width, _height, _windowName.c_str(),
								nullptr, nullptr);
	glfwSetWindowUserPointer(_windowPtr, this);
	glfwSetKeyCallback(_windowPtr, keyCallback);
	glfwSetMouseButtonCallback(_windowPtr, mouseButtonCallback);
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
	_uiContext.destroy();
	if (_surface != VK_NULL_HANDLE)
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
	glfwDestroyWindow(_windowPtr);
	GLFW::release();
}

bool	Window::shouldClose(void) {
	auto	&inputState = _app.getRegistry().getInputState();

	if (inputState.isReleased<input::Key>(GLFW_KEY_ESCAPE) &&
		inputState.getFocused() == this) {
		glfwSetWindowShouldClose(_windowPtr, true);
		inputState.setFocus(this, false);
		return (true);
	}
	if (inputState.isDown<input::Key>(GLFW_KEY_ESCAPE) &&
		inputState.hasMod(GLFW_MOD_CONTROL) &&
		inputState.getFocused() == this) {
		glfwSetWindowShouldClose(_windowPtr, true);
		inputState.setFocus(this, false);
		return (true);
	}
	return (glfwWindowShouldClose(_windowPtr));
}

void	Window::setEntityReference(Entity::id handle) {
	_entityHandle = handle;
}

void	Window::frameBufferResizedCallback(GLFWwindow *window,
												int width, int height) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	appWindow->getSwapchain()._frameBufferResized = true;

	appWindow->_width = static_cast<uint32_t>(width);
	appWindow->_height = static_cast<uint32_t>(height);
}

void	Window::focusCallback(GLFWwindow *window, int focused) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	appWindow->getApp().getRegistry().getInputState().setFocus(appWindow, focused);
}

void	Window::keyCallback(GLFWwindow *window, int key, int scancode,
							int action, int mods) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	if (appWindow->_uiContext.capturesKeyboard())
		return ;
	appWindow->getApp().getRegistry().getInputState().setState<input::Key>(key, action, mods);
}

void	Window::mouseButtonCallback(GLFWwindow *window, int button,
							int action, int mods) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	if (appWindow->_uiContext.capturesMouse())
		return ;
	appWindow->getApp().getRegistry().getInputState().setState<input::Mouse>(button, action, mods);
}

void	Window::cursorEnterCallback(GLFWwindow *window, int entered) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	appWindow->getApp().getRegistry().getInputState().setFocus(appWindow, entered);
}

void	Window::cursorPositionCallback(GLFWwindow *window, double x, double y) {
	auto		appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	appWindow->getApp().getRegistry().getInputState().setMouseMove(appWindow, x, y);
	auto		&input = appWindow->getApp().getRegistry().getInputState();
}

}
