/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InputState.cpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 15:02:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 16:43:46                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/input/InputState.hpp"

#include <iostream>

namespace	hel {

void	InputState::setKeyState(int key, int action, int mods) {
	if (key != GLFW_KEY_UNKNOWN) {
		_current.set(key, action != GLFW_RELEASE);
		_currentMods = mods;
	}
}

void	InputState::setFocus(Window *window, bool focused) {
	if (focused)
		_windowFocused = window;
	else if (_windowFocused == window)
		_windowFocused = nullptr;
}

void	InputState::newFrame(void) {
	_previous = _current;
}

bool	InputState::isKeyHeld(int key) {
	return (_current.test(key));
}

bool	InputState::isKeyPressed(int key) {
	return (_current.test(key) && !_previous.test(key));
}

bool	InputState::isKeyReleased(int key) {
	return (!_current.test(key) && _previous.test(key));
}

bool	InputState::hasMod(int mod) {
	return (_currentMods & mod);
}

bool	InputState::hasAnyChanged(std::vector<int> keys) {
	for (auto key: keys) {
		if (_current.test(key) != _previous.test(key))
			return (true);
	}
	return (false);
}

}
