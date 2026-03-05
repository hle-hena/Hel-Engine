/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/03 11:52:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/05 13:27:38                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/UIHelper.hpp"

#include <algorithm>
#include <iostream>

namespace	hel::sys {

Splitter	&Splitter::setPos(float x, float y) {
	_pos.x = x;
	_pos.y = y;
	return (*this);
}

Splitter	&Splitter::setSize(float size) {
	_size = size;
	return (*this);
}


Splitter	&Splitter::setHitBox(float size) {
	_hitBox = size;
	return (*this);
}

Splitter	&Splitter::setLimits(float minSize, float maxSize) {
	_limits.x = minSize;
	_limits.y = maxSize;
	return (*this);
}

Splitter	&Splitter::setDir(Dir dir) {
	_dir = dir;
	return (*this);
}

Splitter	&Splitter::setVal(float *val) {
	_updateVal = val;
	return (*this);
}

Splitter	&Splitter::setId(const std::string &id) {
	_id = id;
	return (*this);
}

void	Splitter::build(void) {
	if (isHorizontal(_dir)) {
		ImGui::SetNextWindowSize({_size, _hitBox});
		ImGui::SetNextWindowPos(_pos, ImGuiCond_Always, {0.f, 0.5f});
	} else {
		ImGui::SetNextWindowSize({_hitBox, _size});
		ImGui::SetNextWindowPos(_pos, ImGuiCond_Always, {0.5f, 0.f});
	}
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoMove |
							ImGuiWindowFlags_NoScrollbar |
							ImGuiWindowFlags_NoBackground |
							ImGuiWindowFlags_NoSavedSettings;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0, 0});
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
	ImGui::Begin(_id.c_str(), nullptr, flags);

	ImGui::InvisibleButton("##hitarea", ImGui::GetContentRegionAvail());
	bool	active = ImGui::IsItemActive();
	if (active)
		*_updateVal += (isHorizontal(_dir)) ?
						IsPositive(_dir) * ImGui::GetIO().MouseDelta.y :
						IsPositive(_dir) * ImGui::GetIO().MouseDelta.x;
	if (active || ImGui::IsItemHovered())
		ImGui::SetMouseCursor(isHorizontal(_dir) ?
									ImGuiMouseCursor_ResizeNS :
									ImGuiMouseCursor_ResizeEW);

	ImGui::End();
	ImGui::PopStyleVar(2);

	*_updateVal = std::clamp(*_updateVal, _limits.x, _limits.y);
}



DragFloat::DragFloat(GLFWwindow *windowPtr, float *val)
	:	_windowPtr{windowPtr},
		_val{val} {
}

bool	DragFloat::build(void) {
	bool	changed = ImGui::DragFloat(_label, _val, _speed, _min, _max, _format,
					ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
		ImGuiIO	&io= ImGui::GetIO();
		ImVec2	mousePos = io.MousePos;

		float	padding = 3.f;
		float	leftBound = 0.0f;
		float	rightBound = io.DisplaySize.x;

		if (mousePos.x <= leftBound) {
			float newX = rightBound - padding;
			glfwSetCursorPos(_windowPtr, newX, mousePos.y);
			io.MousePosPrev = ImVec2(newX, mousePos.y); 
			io.MousePos = ImVec2(newX, mousePos.y);
		} else if (mousePos.x >= rightBound - 1) {
			float newX = leftBound + padding;
			glfwSetCursorPos(_windowPtr, newX, mousePos.y);
			io.MousePosPrev = ImVec2(newX, mousePos.y);
			io.MousePos = ImVec2(newX, mousePos.y);
		}
	}
	return (changed);
}

}
