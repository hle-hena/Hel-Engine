/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Application.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:12 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/10 20:03:20                                        */
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

namespace	hel {

class	Application {
	public:
		Application(void);
		~Application(void);
		Application(Application &&other) = default;
		Application	&operator=(Application &&other) = default;

		void	run(void);
		void	addNewWindow(int width, int height, const std::string &windowName);

		bool	isAvailable(void) const { return (_available); }

	private:
		Application(const Application &other) = delete;
		Application	&operator=(const Application &other) = delete;

		std::vector<Window::windowPtr>	_appWindows;
		bool							_available{true};
};

}
