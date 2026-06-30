/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Window.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 13:23:29 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/26 11:02:46                                        */
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
# include "platform/ui/UiContext.hpp"

namespace	hel {

class	InputState;
class	VulkanContext;

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
										VulkanContext *context,
										InputState *inputState) noexcept;
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
		void			setEntityReference(Entity::id handle);
		Entity::id		getEntityReference(void) const {
			return (_entityHandle.value_or(Entity::NOT_REGISTERED));
		}
		void			setEntityFocus(Entity::id handle);
		Entity::id		getEntityFocus(void) const {
			return (_focusHandle.value_or(Entity::NOT_REGISTERED));
		}
		bool			focusChanged(void) const {
			return (_focusChanged != 0);
		}
		VkExtent2D		getExtent(void) const {
			return {_width, _height};
		}

		void	pollEvents(void);


	private:
		Window(uint32_t width, uint32_t height, const std::string &windowName,
			VulkanContext *context, InputState *inputState = nullptr);
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
		static void	cursorEnterCallback(GLFWwindow *window, int entered);

		VulkanContext				*_vkCtx{nullptr};
		InputState					*_inputState{nullptr};
		uint32_t					_width;
		uint32_t					_height;
		bool						_frameBufferResized{false};
		std::string					_windowName;
		GLFWwindow					*_windowPtr;
		VkSurfaceKHR				_surface{VK_NULL_HANDLE};
		Swapchain					_swapchain;
		std::optional<Entity::id>	_entityHandle;
		std::optional<Entity::id>	_focusHandle;
		int							_focusChanged{0};

	friend class UiContext;
};

}
