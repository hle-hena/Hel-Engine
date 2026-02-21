/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InputState.tpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:36:02 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/21 15:27:24                                        */
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

template <typename T>
concept	hasInputOffset = requires {
	{ T::OFFSET } -> std::convertible_to<int>;
};

template <typename T>
void	InputState::setState(int index, int action, int mods) {
	if constexpr (requires { T::isValid(index); })
		if (!T::isValid(index))	{ return ; }
	int pos = index;
	if constexpr (hasInputOffset<T>)
		pos += T::OFFSET;
	_current.set(pos, action != GLFW_RELEASE);
	_currentMods = mods;
}

template <typename T>
bool	InputState::isDown(int index) const {
	int	pos = index;
	if constexpr (hasInputOffset<T>) {
		pos += T::OFFSET;
	}
	return (_current.test(pos));
}

template <typename T>
bool	InputState::isPressed(int index) const {
	int	pos = index;
	if constexpr (hasInputOffset<T>)
		pos += T::OFFSET;
	return (!_previous.test(pos) && _current.test(pos));
}

template <typename T>
bool	InputState::isReleased(int index) const {
	int	pos	= index;
	if constexpr (hasInputOffset<T>)
		pos += T::OFFSET;
	return (_previous.test(pos) && !_current.test(pos));
}

template <typename T>
bool	InputState::hasAnyChanged(std::vector<int> indices) const {
	bool	res = false;
	for (auto index: indices) {
		int pos = index;
		if constexpr (hasInputOffset<T>)
			pos += T::OFFSET;
		res += (_previous.test(pos) != _current.test(pos));
	}
	return (res);
}

}
