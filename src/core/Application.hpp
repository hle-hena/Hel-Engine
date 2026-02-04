/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Application.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:12 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 14:30:28                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vector>
# include <memory>
# include <string>

# include "platform/window/Window.hpp"
# include "api/vulkan/VulkanContext.hpp"
# include "core/Engine.hpp"
# include "ecs/Registry.hpp"
# include "ecs/AssetManager.hpp"

namespace	hel {

class	Application {
	public:
		Application(void);
		~Application(void);
		Application(Application &&other) = default;
		Application	&operator=(Application &&other) = default;

		void	run(void);
		void	addNewWindow(int width, int height, const std::string &windowName);

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}
		VulkanContext	&getVkContext(void) {
			return (_vkContext);
		}
		Registry	&getRegistry(void) {
			return (_registry);
		}

	private:
		Application(const Application &other) = delete;
		Application	&operator=(const Application &other) = delete;

		bool							_healthy{true};
		std::string						_reason{""};
		std::vector<Window::windowPtr>	_appWindows;
		VulkanContext					_vkContext;
		Registry						_registry;
		AssetManager					_assetManager;
		Engine							_engine;
};

}
