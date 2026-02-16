/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/16 13:03:31                                        */
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

#include "ecs/system/RenderSystem.hpp"
#include "ecs/system/TransformSystem.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "ecs/system/ControllerSystem.hpp"

#include "api/vulkan/Swapchain.hpp"
#include "api/vulkan/Buffer.hpp"

namespace hel {

class	Window;
class	Device;
class	Registry;

class	Engine {
	public:
		struct	GlobalUBO {
			glm::mat4	viewProjection;
		};

		Engine(Device &device, Registry &registry);
		~Engine();

		Engine(const Engine &) = delete;
		Engine &operator=(const Engine &) = delete;

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}

		bool			init(void);
		void			runFrame(Window &window, uint32_t currentFrame);
		void			updateGlobal(void);

	private:
		struct	WindowResources {
			std::array<VkCommandBuffer, Swapchain::MAX_FRAMES_IN_FLIGHT>			commandBuffers;
			std::array<std::unique_ptr<Buffer>, Swapchain::MAX_FRAMES_IN_FLIGHT>	globalUbos;
		};
		bool			createCommandPool(void);
		bool			createDescriptorSetLayout(void);

		bool			beginFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		bool			endFrame(VkCommandBuffer commandBuffer);
		WindowResources	*getWindowResources(Window& window);

		bool											_healthy{true};
		std::string										_reason{""};
		Device											&_device;
		Registry										&_registry;
		VkCommandPool									_commandPool{VK_NULL_HANDLE};
		VkDescriptorSetLayout							setLayout{VK_NULL_HANDLE};
		std::unordered_map<Window*, WindowResources>	_perWindowResources;
		RenderSystem									_renderSystem;
		TransformSystem									_transformSystem;
		CameraSystem									_cameraSystem;
		ControllerSystem								_controllerSystem;
		uint32_t										_currentFrameIndex{0};
};

}
