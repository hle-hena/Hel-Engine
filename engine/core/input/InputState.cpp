/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InputState.cpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 15:02:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 18:26:00                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/input/InputState.hpp"

namespace	hel {

void	InputState::setMouseMove(double newX, double newY) {
	if (_mousePos.has_value()) {
		_mouseDelta = {
			newX - _mousePos->x,
			newY - _mousePos->y,
		};
	}
	_mousePos = {newX, newY};
}

void	InputState::setFocused(bool focused) {
	_focused = focused;
}

void	InputState::newFrame(std::vector<InputEvent> events) {
	_previous = _current;
	_mouseDelta.reset();

	for (auto &event: events) {
		switch (event.type) {
			case InputEventType::Key:
				setState<input::Key>((size_t)event.index,
					event.action, event.mods); break;
			case InputEventType::MouseButton:
				setState<input::Mouse>((size_t)event.index,
					event.action, event.mods); break;
			case InputEventType::MouseMove:
				setMouseMove(event.x, event.y); break;
			case InputEventType::Focus:
				setFocused(event.focused); break;
		}
	}
}

bool	InputState::hasMod(int mod) {
	return (_currentMods & mod);
}

}
