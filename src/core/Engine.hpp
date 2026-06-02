/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/05/29 17:00:52                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <cassert>

#include "utils/Timer.hpp"

#include "api/vulkan/Swapchain.hpp"
#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Descriptors.hpp"
#include "api/vulkan/ImagePool.hpp"
#include "api/vulkan/Renderer.hpp"

#include "core/Frame.hpp"
#include "core/SystemManager.hpp"

namespace hel {

class	Window;
class	Device;
class	Registry;
class	UiContext;

struct	EngineContext {
	Device		*device;
	Registry	*registry;
	ImagePool	*imagePool;
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

		bool			init(Window &window);
		void			tick(Window *window, uint32_t currentFrame);

	private:
		bool			createCommandPool(void);
		void			createDescriptorPools(void);
		void			createImagePool(void);

		void			updateTick(UiContext &ui, FrameContext &frameCtx);
		void			renderTick(Window *window, UiContext &ui,
								FrameContext &frameCtx);

		void			updateGlobalData(FrameContext &ctx);
		void			writeGlobalData(Renderer &renderer);

		bool											_healthy{true};
		std::string										_reason{""};
		Device											&_device;
		Registry										&_registry;
		Timer											_timer;
		float											_lastFrameTime;
		VkCommandPool									_commandPool{VK_NULL_HANDLE};
		std::unique_ptr<DescriptorPool>					_staticPool;
		std::unique_ptr<ImagePool>						_imagePool;
		Frame											_frame;
		EngineContext									_engineCtx;

		SystemManager									_systems;
};

}
