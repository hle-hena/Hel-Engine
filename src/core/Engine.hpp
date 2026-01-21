/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/21 11:42:10                                        */
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

#include "ecs/system/MeshSystem.hpp"
#include "api/vulkan/Swapchain.hpp"

namespace hel {

class	Window;
class	Device;

class	Engine {
	public:
		Engine(Device &device);
		~Engine();

		Engine(const Engine &) = delete;
		Engine &operator=(const Engine &) = delete;

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}

		bool	init(void);
		void	drawFrame(Window &window, uint32_t currentFrame);

	private:
		using WindowCmdBuffers = std::array<VkCommandBuffer, Swapchain::MAX_FRAMES_IN_FLIGHT>;
		bool	createCommandPool(void);
		bool	createCommandBuffers(void);

		bool			beginFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		bool			endFrame(VkCommandBuffer commandBuffer);
		VkCommandBuffer	getCommandBuffer(Window& window, uint32_t currentFrame);

		bool											_healthy{true};
		std::string										_reason{""};
		Device											&_device;
		VkCommandPool									_commandPool{VK_NULL_HANDLE};
		std::unordered_map<Window*, WindowCmdBuffers>	_perWindowCommandBuffers;
		MeshSystem										_meshSystem;
		uint32_t										_currentFrameIndex{0};

};

}
