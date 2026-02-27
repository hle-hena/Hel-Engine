/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UiContext.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 14:42:09 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 16:49:15                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "api/vulkan/Descriptors.hpp"
# include "api/ImGui/imgui.h"
# include "api/ImGui/imgui_impl_glfw.h"
# include "api/ImGui/imgui_impl_vulkan.h"

namespace	hel {

class	Window;

class	UiContext {
	public:
		UiContext(Window *window);
		~UiContext(void);

		UiContext(const UiContext &other) = delete;
		UiContext	&operator=(const UiContext &other) = delete;

		void	newFrame(VkRenderPass);
		void	renderFrame(VkCommandBuffer commandBuffer);

		ImGuiContext	*get(void) const	{ return (_context); }

	private:
		void	stealAllCallbacks(GLFWwindow *w);
		void	dispatchKey(GLFWwindow *w, int key, int scancode, int action,
							int mods);
		void	dispatchMouse(GLFWwindow *w, int button, int action, int mods);
		void	dispatchScroll(GLFWwindow *w, double xoffset, double yoffset);
		void	disptachCursorPos(GLFWwindow *w, double x, double y);
		void	dispatchCharCallback(GLFWwindow *w, unsigned int c);
		void	dispatchCursorEnter(GLFWwindow *w, int entered);
		void	dispatchFocus(GLFWwindow *w, int focused);

		void	destroy(void);
		void	init(VkRenderPass renderPass);
		void	initDescriptorPool(Device &device);
		void	initImGui(Device &device, VkRenderPass renderPass);

		bool							_fullyInitialised{false};
		std::unique_ptr<DescriptorPool>	_pool;
		ImGuiContext					*_context;
		Window							*_window;

	friend class Window;
};

}
