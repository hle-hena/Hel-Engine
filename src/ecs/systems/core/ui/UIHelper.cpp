/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/03 11:52:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/03 13:31:19                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/UIHelper.hpp"

#include <algorithm>

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
	if (_dir == Vertical) {
		ImGui::SetNextWindowSize({_hitBox, _size});
		_pos.x -= _hitBox / 2.f;
	} else {
		ImGui::SetNextWindowSize({_size, _hitBox});
		_pos.y -= _hitBox / 2.f;
	}
	ImGui::SetNextWindowPos(_pos);
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
	if (active) {
		*_updateVal += (_dir == Vertical) ?
									ImGui::GetIO().MouseDelta.x :
									ImGui::GetIO().MouseDelta.y;
	}
	if (active || ImGui::IsItemHovered()) {
		ImGui::SetMouseCursor((_dir == Vertical) ?
									ImGuiMouseCursor_ResizeEW :
									ImGuiMouseCursor_ResizeNS);

		ImGui::GetForegroundDrawList()->AddRectFilled(
			ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 
			ImGui::GetColorU32(ImGuiCol_SeparatorHovered)
		);
	}

	ImGui::End();
	ImGui::PopStyleVar(2);

	*_updateVal = std::clamp(*_updateVal, _limits.x, _limits.y);
}

}
