/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 13:23:29 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/11 10:04:22                                        */
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
										Application &app) noexcept;
		void				createWindowSurface(VkInstance instance,
												VkSurfaceKHR *surface);

		bool		shouldClose(void) const {
			return (glfwWindowShouldClose(_windowPtr));
		}
		bool		wasResized(void) const {
			return (_frameBufferResized);
		}
		void		resetWindowResizedFlag() {
			_frameBufferResized = false;
		}
		GLFWwindow	*getWindow(void) const {
			return (_windowPtr);
		}
		Application	&getApp(void) const {
			return (_app);
		}
		std::string	getWindowName(void) const {
			return (_windowName);
		}
		VkExtent2D	getExtent(void) const {
			return {
				static_cast<uint32_t>(_width), 
				static_cast<uint32_t>(_height)
			};
		}


	private:
		Window(int width, int height, const std::string &windowName,
			Application &app);
		Window(const Window &other) = delete;
		Window	&operator=(const Window &other) = delete;

		void		initWindow(void);
		void		deleteWindow(void);
		static void	frameBufferResizedCallback(GLFWwindow *window, int width,
											int height);
		

		int			_width;
		int			_height;
		bool		_frameBufferResized{false};
		std::string	_windowName;
		GLFWwindow	*_windowPtr;
		Application	&_app;
};

}
