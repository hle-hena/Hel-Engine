/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UiContext.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 14:42:09 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 19:28:03                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

// #include "api/vulkan/Descriptors.hpp"

#include <memory>
#include <unordered_map>
#include <vulkan/vulkan.h>

#include <ui/ImGui/imgui.h>

namespace	hel {

class	Window;
class	Device;
class	DescriptorPool;
struct	DescriptorSet;

class	UiContext {
	public:
		~UiContext(void);

		UiContext(const UiContext &other) = delete;
		UiContext	&operator=(const UiContext &other) = delete;

		static VkDescriptorSet	registerTexture(Device &device,
										VkSampler sampler,
										VkImageView view);
		static void				unregisterTexture(VkDescriptorSet texture);

		static void	newFrame(void);
		static void	endFrame(void);
		static void	renderFrame(VkCommandBuffer commandBuffer);

		ImGuiContext	*get(void) const	{ return (_context); }

	private:
		static bool	capturesMouse(void);
		static bool	capturesKeyboard(void);

		static void	destroy(Device *device);
		static void	init(Window *window, Device *device);
		static void	initDescriptorPool(Device *device);
		static void	initImGui(Window *window, Device *device);
		static void	initImGuiStyle(void);

		static bool									_fullyInitialised;
		static std::unique_ptr<DescriptorPool>		_pool;
		static std::unordered_map<
					VkDescriptorSet,
					std::unique_ptr<DescriptorSet>>	_textures;
		static ImGuiContext							*_context;

	friend class Window;
};

}
