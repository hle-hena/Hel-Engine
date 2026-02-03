/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 13:23:29 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/03 12:02:25                                        */
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
# include <optional>

# include "api/vulkan/Swapchain.hpp"
# include "ecs/Entity.hpp"


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
		static windowPtr	createBootstrap(int width, int height,
										const std::string &windowName,
										Application &app, VkInstance &instance) noexcept;
		bool				shouldClose(void);

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}
		GLFWwindow		*getWindow(void) const {
			return (_windowPtr);
		}
		VkSurfaceKHR	&getSurface(void) {
			return (_surface);
		}
		Application		&getApp(void) const {
			return (_app);
		}
		Swapchain		&getSwapchain(void) {
			return (_swapchain);
		}
		std::string		getWindowName(void) const {
			return (_windowName);
		}
		VkFormat		getFormat(void) const {
			return (_swapchain.getFormat());
		}
		void	setEntityReference(Entity::id handle) {
			_entityHandle = handle;
		}
		Entity::id	getEntityReference(void) const {
			return (_entityHandle.value_or(Entity::NOT_REGISTERED));
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
		static void	keyCallback(GLFWwindow *window, int key, int scancode,
								int action, int mods);
		static void	focusCallback(GLFWwindow *window, int focused);
		static void cursorPositionCallback(GLFWwindow* window, double xpos,
										double ypos);
		static void	cursorEnterCallback(GLFWwindow *window, int entered);

		bool						_healthy{true};
		std::string					_reason{""};
		int							_width;
		int							_height;
		int							_lastMouseX{-1};
		int							_lastMouseY{-1};
		bool						_frameBufferResized{false};
		std::string					_windowName;
		GLFWwindow					*_windowPtr;
		VkSurfaceKHR				_surface{VK_NULL_HANDLE};
		Swapchain					_swapchain;
		Application					&_app;
		VkInstance					&_instance;
		std::optional<Entity::id>	_entityHandle;
};

}
