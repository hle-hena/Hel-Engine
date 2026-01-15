/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 16:35:00 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 19:35:21                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <cassert>

#include "render/system/MeshSystem.hpp"
#include "render/vulkan/Swapchain.hpp"

namespace hel {

class	Window;
class	Device;

class	Renderer {
	public:
		Renderer(Device &device);
		~Renderer();

		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}

		bool	init(void);
		void	drawFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex, Window &window);

	private:
		bool	createCommandPool(void);
		bool	createCommandBuffers(void);

		bool	beginFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		bool	endFrame(VkCommandBuffer commandBuffer);

		bool							_healthy{true};
		std::string						_reason{""};
		Device							&_device;
		VkCommandPool					_commandPool{VK_NULL_HANDLE};
		std::vector<VkCommandBuffer>	_commandBuffers;
		MeshSystem						_meshSystem;
		uint32_t						_currentFrameIndex{0};

};

}
