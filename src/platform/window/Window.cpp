/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 12:20:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 15:39:10                                        */
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

void	Window::frameBufferResizedCallback(GLFWwindow *window,
												int width, int height) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	appWindow->getSwapchain()._frameBufferResized = true;
}

void	Window::keyCallback(GLFWwindow *window, int key, int scancode,
							int action, int mod) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	auto	&app = appWindow->getApp();
	auto	&appRegistry = app.getRegistry();

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		return ;
	}
	else if (key == GLFW_KEY_N && action == GLFW_PRESS
			&& mod == GLFW_MOD_CONTROL) {
		app.addNewWindow(Window::WIDTH, Window::HEIGHT, appWindow->getWindowName() + "_copy");
		return ;
	}
	Entity::id	entityHandle = appWindow->getEntityReference();
	if (entityHandle == Entity::NOT_REGISTERED)
		return ;
	appRegistry.patch(appRegistry.getComponent<Transform>(entityHandle), [&](Transform &transform){
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
			transform.position.y += 1;
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
			transform.position.y -= 1;
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
			transform.position.x += 1;
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
			transform.position.x -= 1;
	});
	appRegistry.patch(appRegistry.getComponent<Camera>(entityHandle), [](Camera &cam){});
}

}
