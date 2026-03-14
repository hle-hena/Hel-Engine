/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 22:50:37                                        */
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
#include "platform/ui/UI.hpp"

#include "ecs/systems/runtime/BaseController.hpp"
#include "ecs/systems/runtime/SurfaceAllignement.hpp"

#include "utils/Timer.hpp"

#include "api/vulkan/Swapchain.hpp"
#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Descriptors.hpp"
#include "api/vulkan/ImagePool.hpp"

#include "core/Frame.hpp"

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
		void			createDescriptorPool(void);
		void			createImagePool(void);

		void			UITick(UiContext &ui, const FrameContext &frameCtx);
		void			updateTick(const FrameContext &frameCtx);
		void			renderTick(Window *window, UiContext &ui,
								const FrameContext &frameCtx,
								uint32_t frameIndex);

		void			updateGlobalUBO(Window &window, uint32_t currentFrame);

		bool											_healthy{true};
		std::string										_reason{""};
		Device											&_device;
		Registry										&_registry;
		Timer											_timer;
		float											_lastFrameTime;
		VkCommandPool									_commandPool{VK_NULL_HANDLE};
		std::unique_ptr<DescriptorPool>					_staticPool;
		std::unique_ptr<ImagePool>						_imagePool;
		std::vector<std::unique_ptr<sys::ISystem>>		_systems;
		Frame											_frame;
		EngineContext									_engineCtx;
};

}
