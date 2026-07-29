/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 12:20:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 19:20:11                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/window/Window.hpp"
#include "rhi/window/UiContext.hpp"
#include "rhi/window/GLFW.hpp"
#include "rhi/context/VulkanContext.hpp"

namespace	hel {

Window::windowPtr	Window::createWindow(uint32_t width, uint32_t height,
										const std::string &windowName,
										VulkanContext *ctx) noexcept {
	if (!GLFW::acquire())
		return (nullptr);
	try {
		Window::windowPtr	window = Window::windowPtr(new Window(width, height,
																windowName, ctx));
		if (glfwCreateWindowSurface(ctx->getInstance(), window->getWindow(),
									nullptr, &window->_surface) != VK_SUCCESS)
			return (nullptr);
		if (auto res = window->_swapchain.initiateSwapChain(*window); !res) {
			std::cerr << res.error() << std::endl;
			return (nullptr);
		}
		UiContext::init(window.get(), ctx->getDevice());
		return (window);
	} catch (...) {
		GLFW::release();
		return (nullptr);
	}
}

Window::windowPtr	Window::createBootstrap(uint32_t width, uint32_t height,
										const std::string &windowName,
										VulkanContext *ctx) noexcept {
	if (!GLFW::acquire())
		return (nullptr);
	try {
		Window::windowPtr	window = Window::windowPtr(new Window(width, height,
																windowName, ctx));
		if (glfwCreateWindowSurface(ctx->getInstance(), window->getWindow(),
									nullptr, &window->_surface) != VK_SUCCESS)
			return (nullptr);
		return (window);
	} catch (...) {
		GLFW::release();
		return (nullptr);
	}
}

Window::Window(uint32_t width, uint32_t height, const std::string &windowName,
			VulkanContext *ctx)
	:	_vkCtx(ctx),
		_width(width),
		_height(height),
		_windowName(windowName),
		_windowPtr(nullptr),
		_swapchain{*ctx->getDevice()} {
	initWindow();
}

void	Window::initWindow(void) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	_windowPtr = glfwCreateWindow(static_cast<int>(_width),
		static_cast<int>(_height), _windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(_windowPtr, this);
	glfwSetKeyCallback(_windowPtr, keyCallback);
	glfwSetMouseButtonCallback(_windowPtr, mouseButtonCallback);
	glfwSetFramebufferSizeCallback(_windowPtr, frameBufferResizedCallback);
	glfwSetWindowFocusCallback(_windowPtr, focusCallback);
	glfwSetCursorPosCallback(_windowPtr, cursorPositionCallback);
}

Window::~Window(void) {
	deleteWindow();
}

void	Window::deleteWindow(void) {
	_swapchain.deleteSwapChain();
	UiContext::destroy(_vkCtx->getDevice());
	if (_surface != VK_NULL_HANDLE)
		vkDestroySurfaceKHR(_vkCtx->getInstance(), _surface, nullptr);
	glfwDestroyWindow(_windowPtr);
	GLFW::release();
}

std::vector<InputEvent>	Window::pollEvents(void) {
	glfwPollEvents();
	return std::exchange(_pendingEvents, {});
}

bool	Window::shouldClose(void) {
	return (glfwWindowShouldClose(_windowPtr));
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

	appWindow->_pendingEvents.push_back(InputEvent::focus(focused));
}

void	Window::keyCallback(GLFWwindow *window, int key, int,
							int action, int mods) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	if (action != GLFW_RELEASE && UiContext::capturesKeyboard())
		return ;
	appWindow->_pendingEvents.push_back(
		InputEvent::keyPressed(key, action, mods));
}

void	Window::mouseButtonCallback(GLFWwindow *window, int button,
							int action, int mods) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	if (action != GLFW_RELEASE && UiContext::capturesMouse())
		return ;
	appWindow->_pendingEvents.push_back(
		InputEvent::mouseButton(button, action, mods));
}

void	Window::cursorPositionCallback(GLFWwindow *window, double x, double y) {
	auto		appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	appWindow->_pendingEvents.push_back(InputEvent::mouseMove(x, y));
}

}
