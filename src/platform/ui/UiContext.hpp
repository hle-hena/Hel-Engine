/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UiContext.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 14:42:09 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/05 17:34:27                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "api/vulkan/Descriptors.hpp"

# include <ui/ImGui/imgui.h>
# include <ui/ImGui/imgui_impl_glfw.h>
# include <ui/ImGui/imgui_impl_vulkan.h>

namespace	hel {

class	Window;
class	Device;
class	Image;

class	UiContext {
	public:
		UiContext(Window *window);
		~UiContext(void);

		UiContext(const UiContext &other) = delete;
		UiContext	&operator=(const UiContext &other) = delete;

		static VkDescriptorSet	registerTexture(Device &device, Image *image,
									VkFormat format, VkImageAspectFlags aspect);
		static void				unregisterTexture(VkDescriptorSet texture);

		void	newFrame();
		void	endFrame(void);
		void	renderFrame(VkCommandBuffer commandBuffer);

		ImGuiContext	*get(void) const	{ return (_context); }

	private:
		bool	capturesMouse(void);
		bool	capturesKeyboard(void);

		void	destroy(void);
		void	init();
		void	initDescriptorPool(Device &device);
		void	initImGui(Device &device);
		void	initImGuiStyle(void);

		bool							_fullyInitialised{false};
		std::unique_ptr<DescriptorPool>	_pool;
		ImGuiContext					*_context;
		Window							*_window;
		Device							&_device;

	friend class Window;
};

}
