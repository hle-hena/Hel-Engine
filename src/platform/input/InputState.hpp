/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InputState.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 15:01:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/03 11:38:31                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>
# include <bitset>
# include <vector>

namespace	hel {

class	Window;

class	InputState {
	public:
		InputState(void) = default;
		~InputState(void) = default;
		InputState(const InputState &) = delete;
		InputState	&operator=(const InputState &) = delete;

		void	newFrame(void);

		bool	isKeyHeld(int key);
		bool	isKeyPressed(int key);
		bool	isKeyReleased(int key);
		bool	hasMod(int mod);
		bool	hasAnyChanged(std::vector<int> keys);
		bool	mouseMoved(void) { return (_mouseDeltaX || _mouseDeltaY); }

		Window	*getFocused(void) const {
			return (_windowFocused);
		}
		void	getMouseDelta(int &deltaX, int &deltaY) {
			deltaX = _mouseDeltaX;
			deltaY = _mouseDeltaY;
		}

	private:
		void	setKeyState(int key, int action, int mods);
		void	setFocus(Window *window, bool focused);
		void	setMouseMove(Window *window, double deltaX, double deltaY);

		std::bitset<512>	_current{false};
		std::bitset<512>	_previous{false};
		int					_currentMods{0};
		int					_mouseDeltaY{0};
		int					_mouseDeltaX{0};
		Window				*_windowFocused{nullptr};

	friend class Window;
};

}
