/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InputState.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 15:01:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 11:23:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <bitset>
#include <optional>

namespace	hel::input {

struct	Key {
	static constexpr int OFFSET = 0;
	static bool	isValid(int id)	{ return (id != GLFW_KEY_UNKNOWN); }
};
struct	Mouse {
	static constexpr int OFFSET = 400;
	static bool	isValid(int id)	{ return (id <= GLFW_MOUSE_BUTTON_LAST); }
};

}

namespace	hel {

class	Window;

class	InputState {
	public:
		InputState(void) = default;
		~InputState(void) = default;
		InputState(const InputState &) = delete;
		InputState	&operator=(const InputState &) = delete;

		void	newFrame(void);

		template <typename T>
		bool	isDown(size_t index) const;
		template <typename T>
		bool	isPressed(size_t index) const;
		template <typename T>
		bool	isReleased(size_t index) const;
		template <typename T>
		bool	hasAnyChanged(std::vector<size_t> indices) const;

		bool	hasMod(int mod);
		bool	mouseMoved(void) { return (_mouseDelta.has_value()); }

		Window	*getFocused(void) const {
			return (_windowFocused);
		}
		glm::vec2	getMousePos(void) {
			return (_mousePos.value_or(glm::vec2{-1.f, -1.f}));
		}
		glm::vec2	getMouseDelta(void) {
			return (_mouseDelta.value_or(glm::vec2{0.f, 0.f}));
		}

		void	resetMousePos(const glm::vec2 &newMousePos) {
			_mouseDelta.reset();
			_mousePos = newMousePos;
		}
		void	resetMousePos(void) {
			_mouseDelta.reset();
			_mousePos.reset();
		}

	private:
		template <typename T>
		void	setState(size_t index, int action, int mods);
		void	setFocus(Window *window, bool focused);
		void	setMouseMove(double newX, double newY);

		std::bitset<512>			_current{false};
		std::bitset<512>			_previous{false};
		int							_currentMods{0};
		std::optional<glm::vec2>	_mouseDelta;
		std::optional<glm::vec2>	_mousePos;
		Window						*_windowFocused{nullptr};

	friend class Window;
};

}

#include "core/input/InputState.tpp"
