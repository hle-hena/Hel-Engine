/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InputState.cpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 15:02:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/01 17:14:30                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/input/InputState.hpp"

namespace	hel {

void	InputState::setFocus(Window *window, bool focused) {
	if (focused)
		_windowFocused = window;
	else if (_windowFocused == window)
		_windowFocused = nullptr;
}

void	InputState::setMouseMove(Window *window, double newX, double newY) {
	if (_mousePos.has_value()) {
		_mouseDelta = {
			newX - _mousePos->x,
			newY - _mousePos->y,
		};
	}
	_mousePos = {newX, newY};
}

void	InputState::newFrame(void) {
	_previous = _current;
	_mouseDelta.reset();
}

bool	InputState::hasMod(int mod) {
	return (_currentMods & mod);
}

}
