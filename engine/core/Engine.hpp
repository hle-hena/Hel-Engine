/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/24 17:34:45 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/27 20:50:48                                        */
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
#include "utils/expected.hpp"
#include "core/Frame.hpp"
#include "platform/input/InputState.hpp"
#include "api/vulkan/ImagePool.hpp"

namespace	hel {

class	ImagePool;
class	Window;

struct	EngineConfig {
	EngineConfig(std::vector<UserData> *globalUserData)
		:	globalUserData(globalUserData) {}

	std::function<expected<GlobalSetBindings>(void)>	defineGlobalSet;
	std::function<void(Registry*, Window*)>				loadPrimaryScene;
	std::function<void(Registry *, FrameContext &)>		updateGlobalData;

	std::vector<UserData>	*globalUserData;
};

class	Engine {
	public:
		Engine(void);
		~Engine(void);

		tl::expected<void, std::string>	init(const EngineConfig &config);
		void	run(void);
	
	private:
		tl::expected<void, std::string>	createWindow(int width, int height,
												const std::string &windowName);
		tl::expected<void, std::string>	createImagePool(void);

		void	tick(uint32_t frameIndex);
		void	updateTick(FrameContext &frameCtx);
		void	renderTick(Window *window, FrameContext &ctx);
		void	executePass(FrameContext &ctx,
							const SystemManager::FuncVec &funcs);

		EngineConfig					_config{nullptr};
		VulkanContext					_vkContext;
		Device							*_device{nullptr};
		std::unique_ptr<Window>			_appWindow{nullptr};
		Registry						_registry;
		InputState						_inputState;
		VkCommandPool					_commandPool;
		std::unique_ptr<ImagePool>		_imagePool;
		SystemManager					_systems;
		Frame							_frame;
};

}
