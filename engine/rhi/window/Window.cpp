/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 12:20:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 10:30:17                                        */
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

#include "core/input/InputState.hpp"//remove. aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

namespace	hel {

Window::windowPtr	Window::createWindow(uint32_t width, uint32_t height,
										const std::string &windowName,
										VulkanContext *ctx,
										InputState *inputState) noexcept {
	if (!GLFW::acquire())
		return (nullptr);
	try {
		Window::windowPtr	window = Window::windowPtr(new Window(width, height,
																windowName, ctx,
																inputState));
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
			VulkanContext *ctx, InputState *inputState)
	:	_vkCtx(ctx),
		_inputState(inputState),
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
	glfwSetCursorEnterCallback(_windowPtr, cursorEnterCallback);
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

void	Window::pollEvents(void) {
	glfwPollEvents();
	if (_focusChanged > 0)
		--_focusChanged;
}

bool	Window::shouldClose(void) {
	//TODO -> this should be on the user side. Only default should be the button.
	if (_inputState->isReleased<input::Key>(GLFW_KEY_ESCAPE) &&
		_inputState->getFocused() == this) {
		glfwSetWindowShouldClose(_windowPtr, true);
		_inputState->setFocus(this, false);
		return (true);
	}
	if (_inputState->isDown<input::Key>(GLFW_KEY_ESCAPE) &&
		_inputState->hasMod(GLFW_MOD_CONTROL) &&
		_inputState->getFocused() == this) {
		glfwSetWindowShouldClose(_windowPtr, true);
		_inputState->setFocus(this, false);
		return (true);
	}
	return (glfwWindowShouldClose(_windowPtr));
}

void	Window::setEntityReference(Entity::id handle) {
	_entityHandle = handle;
}

void	Window::setEntityFocus(Entity::id handle) {
	_focusHandle = handle;
	_focusChanged = Swapchain::MAX_FRAMES_IN_FLIGHT;
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

	appWindow->_inputState->setFocus(appWindow, focused);
}

void	Window::keyCallback(GLFWwindow *window, int key, int,
							int action, int mods) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	if (action != GLFW_RELEASE && UiContext::capturesKeyboard())
		return ;
	appWindow->_inputState
		->setState<input::Key>(static_cast<size_t>(key), action, mods);
}

void	Window::mouseButtonCallback(GLFWwindow *window, int button,
							int action, int mods) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	if (action != GLFW_RELEASE && UiContext::capturesMouse())
		return ;
	appWindow->_inputState
		->setState<input::Mouse>(static_cast<size_t>(button), action, mods);
}

void	Window::cursorEnterCallback(GLFWwindow *window, int entered) {
	auto	appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	appWindow->_inputState->setFocus(appWindow, entered);
}

void	Window::cursorPositionCallback(GLFWwindow *window, double x, double y) {
	auto		appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	appWindow->_inputState->setMouseMove(x, y);
}

}
