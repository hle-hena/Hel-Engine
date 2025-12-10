/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Hel.hpp                                                             */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:12 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/10 16:26:55                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "platform/window/Window.hpp"

namespace	hel {

class	Application {
	public:
		Application(void);
		~Application(void);
		Application(Application &&other) = default;
		Application	&operator=(Application &&other) = default;

		void	run(void);

		bool	isUnavailable(void) const { return (_unavailable); }

		private:
		Application(const Application &other) = delete;
		Application	&operator=(const Application &other) = delete;

		Window::windowPtr	_helWindow;
		bool				_unavailable{false};
};

}
