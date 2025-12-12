/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Application.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:12 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/11 10:54:46                                        */
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
# include "render/vulkan/Device.hpp"

namespace	hel {

class	Application {
	public:
		Application(void);
		~Application(void);
		Application(Application &&other) = default;
		Application	&operator=(Application &&other) = default;

		void	run(void);
		void	addNewWindow(int width, int height, const std::string &windowName);

		bool	isHealthy(void) const { return (_healthy); }

	private:
		Application(const Application &other) = delete;
		Application	&operator=(const Application &other) = delete;

		bool							_healthy{true};
		std::vector<Window::windowPtr>	_appWindows;
		Device							_appDevice;
};

}
