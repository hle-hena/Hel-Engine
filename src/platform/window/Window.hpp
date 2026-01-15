/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 13:23:29 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 19:10:18                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

# pragma once

# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>
# include <string>
# include <memory>

# include "render/vulkan/Swapchain.hpp"


namespace	hel {

class	Application;

class	Window {
	public:
		static constexpr int	WIDTH = 800;
		static constexpr int	HEIGHT = 600;
		using windowPtr = std::unique_ptr<Window>;

		Window(Window &&other) = default;
		Window	&operator=(Window &&other) = default;
		~Window(void);

		static windowPtr	createWindow(int width, int height,
										const std::string &windowName,
										Application &app, VkInstance &instance) noexcept;
		static windowPtr		createBootstrap(int width, int height,
										const std::string &windowName,
										Application &app, VkInstance &instance) noexcept;

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}
		bool			shouldClose(void) const {
			return (glfwWindowShouldClose(_windowPtr));
		}
		bool			wasResized(void) const {
			return (_frameBufferResized);
		}
		void			resetWindowResizedFlag() {
			_frameBufferResized = false;
		}
		GLFWwindow		*getWindow(void) const {
			return (_windowPtr);
		}
		VkSurfaceKHR	&getSurface(void) {
			return (_surface);
		}
		Application	&getApp(void) const {
			return (_app);
		}
		Swapchain	&getSwapchain(void) {
			return (_swapchain);
		}
		std::string		getWindowName(void) const {
			return (_windowName);
		}
		VkExtent2D		getExtent(void) const {
			return {
				static_cast<uint32_t>(_width), 
				static_cast<uint32_t>(_height)
			};
		}
		VkFormat		getFormat(void) const {
			return (_swapchain.getFormat());
		}


	private:
		Window(int width, int height, const std::string &windowName,
			Application &app, VkInstance &instance);
		Window(const Window &other) = delete;
		Window	&operator=(const Window &other) = delete;

		void		initWindow(void);
		void		deleteWindow(void);
		static void	frameBufferResizedCallback(GLFWwindow *window, int width,
											int height);

		bool			_healthy{true};
		std::string		_reason{""};
		int				_width;
		int				_height;
		bool			_frameBufferResized{false};
		std::string		_windowName;
		GLFWwindow		*_windowPtr;
		VkSurfaceKHR	_surface{VK_NULL_HANDLE};
		Swapchain		_swapchain;
		Application		&_app;
		VkInstance		&_instance;
};

}
