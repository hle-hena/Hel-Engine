/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 13:23:29 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 19:19:31                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <optional>

#include "rhi/context/Swapchain.hpp"

namespace	hel {

class	VulkanContext;

enum class	InputEventType { Key, MouseButton, MouseMove, Focus };

struct	InputEvent {
	static InputEvent	mouseButton(int index, int action, int mods) {
		InputEvent	ret{};
		ret.type = InputEventType::MouseButton;
		ret.index = index;
		ret.action = action;
		ret.mods = mods;
		return ret;
	}
	static InputEvent	mouseMove(double xpos, double ypos) {
		InputEvent	ret{};
		ret.type = InputEventType::MouseMove;
		ret.x = xpos;
		ret.y = ypos;
		return ret;
	}
	static InputEvent	keyPressed(int key, int action, int mods) {
		InputEvent	ret{};
		ret.type = InputEventType::Key;
		ret.index = key;
		ret.action = action;
		ret.mods = mods;
		return ret;
	}
	static InputEvent	focus(bool focused) {
		InputEvent	ret{};
		ret.type = InputEventType::Focus;
		ret.focused = focused;
		return ret;
	}

	InputEventType	type;
	int				index{0};
	int				action{0};
	int				mods{0};
	double			x{0.f}, y{0.f};
	bool			focused{false};
};

class	Window {
	public:
		static constexpr int	WIDTH = 800;
		static constexpr int	HEIGHT = 600;
		using windowPtr = std::unique_ptr<Window>;

		Window(Window &&other) = delete;
		Window	&operator=(Window &&other) = delete;
		~Window(void);

		static windowPtr	createWindow(uint32_t width, uint32_t height,
										const std::string &windowName,
										VulkanContext *context) noexcept;
		static windowPtr	createBootstrap(uint32_t width, uint32_t height,
										const std::string &windowName,
										VulkanContext *context) noexcept;
		bool				shouldClose(void);

		GLFWwindow		*getWindow(void) const {
			return (_windowPtr);
		}
		VkSurfaceKHR	&getSurface(void) {
			return (_surface);
		}
		Swapchain		&getSwapchain(void) {
			return (_swapchain);
		}
		std::string		getWindowName(void) const {
			return (_windowName);
		}
		VkExtent2D		getExtent(void) const {
			return {_width, _height};
		}

		std::vector<InputEvent>	pollEvents(void);


	private:
		Window(uint32_t width, uint32_t height, const std::string &windowName,
			VulkanContext *context);
		Window(const Window &other) = delete;
		Window	&operator=(const Window &other) = delete;

		void		initWindow(void);
		void		deleteWindow(void);
		static void	frameBufferResizedCallback(GLFWwindow *window, int width,
											int height);
		static void	keyCallback(GLFWwindow *window, int key, int scancode,
								int action, int mods);
		static void	mouseButtonCallback(GLFWwindow *window, int button,
								int action, int mods);
		static void	focusCallback(GLFWwindow *window, int focused);
		static void cursorPositionCallback(GLFWwindow* window, double xpos,
										double ypos);

		VulkanContext				*_vkCtx{nullptr};
		uint32_t					_width;
		uint32_t					_height;
		bool						_frameBufferResized{false};
		std::string					_windowName;
		GLFWwindow					*_windowPtr;
		VkSurfaceKHR				_surface{VK_NULL_HANDLE};
		Swapchain					_swapchain;

		std::vector<InputEvent>		_pendingEvents;

	friend class UiContext;
};

}
