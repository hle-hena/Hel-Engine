/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UiContext.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 14:42:09 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/28 13:20:44                                        */
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
		void	endFrame(void);
		void	renderFrame(VkCommandBuffer commandBuffer);

		ImGuiContext	*get(void) const	{ return (_context); }

	private:
		bool	capturesMouse(void);
		bool	capturesKeyboard(void);

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
