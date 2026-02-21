/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/21 15:33:24                                        */
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

#include "ecs/systems/core/Render.hpp"
#include "ecs/systems/core/Transform.hpp"
#include "ecs/systems/core/Camera.hpp"
#include "ecs/systems/core/EditorController.hpp"
#include "ecs/systems/core/HideMouse.hpp"

#include "ecs/systems/runtime/BaseController.hpp"
#include "ecs/systems/runtime/SurfaceAllignement.hpp"

#include "utils/Timer.hpp"

#include "api/vulkan/Swapchain.hpp"
#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/RenderPass.hpp"

namespace hel {

class	Window;
class	Device;
class	Registry;

struct	GlobalUBO {
	glm::mat4	viewProjection;
	float		elapsedTime;
};

struct	WindowResources {
	Window																	*window;
	std::array<VkCommandBuffer, Swapchain::MAX_FRAMES_IN_FLIGHT>			commandBuffers{};
	std::array<std::unique_ptr<Buffer>, Swapchain::MAX_FRAMES_IN_FLIGHT>	globalUbos{};
	std::array<VkDescriptorSet, Swapchain::MAX_FRAMES_IN_FLIGHT>			globalDescriptorSets{};
};

class	Engine {
	public:
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
		void			updateFrame(void);
		void			renderFrame(Window &window, uint32_t currentFrame);

	private:
		bool			createCommandPool(void);
		bool			createDescriptorSetLayout(void);
		bool			createDescriptorPool(void);

		void			updateGlobalUBO(Window &window, uint32_t currentFrame);
		bool			beginFrame(VkRenderPass renderPass,
								VkCommandBuffer commandBuffer,
								VkFramebuffer framebuffer, VkExtent2D extent);
		bool			endFrame(VkCommandBuffer commandBuffer);
		WindowResources	*getWindowResources(Window& window);

		bool											_healthy{true};
		std::string										_reason{""};
		Device											&_device;
		Registry										&_registry;
		Timer											_timer;
		float											_lastFrameTime;
		VkCommandPool									_commandPool{VK_NULL_HANDLE};
		VkDescriptorPool								_descriptorPool{VK_NULL_HANDLE};
		VkDescriptorSetLayout							_setLayout{VK_NULL_HANDLE};
		RenderPass										_passes;
		std::unordered_map<Window*, WindowResources>	_perWindowResources;
		sys::Render										_renderSystem;
		sys::Transform									_transformSystem;
		sys::Camera										_cameraSystem;
		sys::HideMouse									_hideMouseSystem;
		sys::EditorController							_editorControllerSystem;
		sys::BaseController								_baseControllerSystem;
		sys::SurfaceAllignement							_surfaceAllignementSystem;
		uint32_t										_currentFrameIndex{0};
};

}
