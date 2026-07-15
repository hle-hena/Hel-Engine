/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/24 17:34:45 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/15 15:08:40                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string>

#include "api/vulkan/VulkanContext.hpp"
#include "ecs/Registry.hpp"
#include "core/SystemManager.hpp"
#include "core/Frame.hpp"
#include "platform/input/InputState.hpp"
#include "HelExpected.hpp"

namespace	hel {

class	ImagePool;
class	Window;

struct	EngineConfig {
	std::function<GlobalSetBindings(void)>			defineGlobalSet;
	std::function<void(Registry*, Window*)>			loadPrimaryScene;
	std::function<void(Registry*, FrameContext&)>	updateGlobalData;
	std::function<void(Registry*, FrameContext&)>	tickCallback;
};

class	Engine {
	public:
		Engine(void);
		~Engine(void);

		expected<void>	init(const EngineConfig &config);
		expected<void>	setUserData(std::shared_ptr<GlobalData> userData);
		void	run(void);

		Device	*device(void)	{ return _device; };
	
	private:
		expected<void>	createWindow(int width, int height,
									const std::string &windowName);
		expected<void>	createImagePool(void);

		void	tick(uint32_t frameIndex);
		void	updateTick(FrameContext &frameCtx);
		void	renderTick(Window *window, FrameContext &ctx);
		void	executePass(FrameContext &ctx,
							const SystemManager::EntryVec &funcs);

		EngineConfig				_config;
		std::shared_ptr<GlobalData>	_userData;

		VulkanContext				_vkContext;
		Device						*_device{nullptr};
		std::unique_ptr<Window>		_appWindow{nullptr};
		Registry					_registry;
		InputState					_inputState;
		std::unique_ptr<ImagePool>	_imagePool;
		SystemManager				_systems;
		Frame						_frame;
};

}
