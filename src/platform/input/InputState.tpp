/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InputState.tpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:36:02 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/30 21:05:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/input/InputState.hpp"

namespace	hel {

template <typename T>
concept	hasInputOffset = requires {
	{ T::OFFSET } -> std::convertible_to<int>;
};

template <typename T>
void	InputState::setState(size_t index, int action, int mods) {
	if constexpr (requires { T::isValid(index); })
		if (!T::isValid(index))	{ return ; }
	size_t pos = index;
	if constexpr (hasInputOffset<T>)
		pos += T::OFFSET;
	_current.set(pos, action != GLFW_RELEASE);
	_currentMods = mods;
}

template <typename T>
bool	InputState::isDown(size_t index) const {
	size_t	pos = index;
	if constexpr (hasInputOffset<T>)
		pos += T::OFFSET;
	return (_current.test(pos));
}

template <typename T>
bool	InputState::isPressed(size_t index) const {
	size_t	pos = index;
	if constexpr (hasInputOffset<T>)
		pos += T::OFFSET;
	return (!_previous.test(pos) && _current.test(pos));
}

template <typename T>
bool	InputState::isReleased(size_t index) const {
	size_t	pos	= index;
	if constexpr (hasInputOffset<T>)
		pos += T::OFFSET;
	return (_previous.test(pos) && !_current.test(pos));
}

template <typename T>
bool	InputState::hasAnyChanged(std::vector<size_t> indices) const {
	bool	res = false;
	for (auto index: indices) {
		size_t pos = index;
		if constexpr (hasInputOffset<T>)
			pos += T::OFFSET;
		res += (_previous.test(pos) != _current.test(pos));
	}
	return (res);
}

}
