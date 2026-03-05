/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/03 11:52:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/05 17:59:28                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/UIHelper.hpp"
#include "api/ImGui/imgui_stdlib.h"

#include <algorithm>
#include <iostream>

namespace	hel::sys {

Splitter::Splitter(float *val)
	:	_val{val} {
}

void	Splitter::build(void) {
	if (isHorizontal(_dir)) {
		ImGui::SetNextWindowSize({_size, _hitbox});
		ImGui::SetNextWindowPos(_pos, ImGuiCond_Always, {0.f, 0.5f});
	} else {
		ImGui::SetNextWindowSize({_hitbox, _size});
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
	ImGui::Begin(_label, nullptr, flags);

	ImGui::InvisibleButton("##hitarea", ImGui::GetContentRegionAvail());
	bool	active = ImGui::IsItemActive();
	if (active)
		*_val += (isHorizontal(_dir)) ?
						IsPositive(_dir) * ImGui::GetIO().MouseDelta.y :
						IsPositive(_dir) * ImGui::GetIO().MouseDelta.x;
	if (active || ImGui::IsItemHovered())
		ImGui::SetMouseCursor(isHorizontal(_dir) ?
									ImGuiMouseCursor_ResizeNS :
									ImGuiMouseCursor_ResizeEW);

	ImGui::End();
	ImGui::PopStyleVar(2);

	*_val = std::clamp(*_val, _min, _max);
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



Table::Table(const char *name)
	:	_name{name} {
}



bool	Table::begin(uint32_t col) {
	if (ImGui::BeginTable(_name, col * 2 + 1, ImGuiTableFlags_SizingFixedFit)) {
		ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
		for (uint32_t i = 0; i < col; i++) {
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);
		}
		return (true);
	}
	return (false);
}

void	Table::end(void) {
	ImGui::EndTable();
}

void	Table::newRow(const char *rowName) {
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::AlignTextToFramePadding();
	ImGui::Text(rowName);
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(10.0f, 0.0f));
}



const std::unordered_map<TableRow::Type, TableRow::BuildFunc>
		TableRow::_buildFunctions = {
			{ TableRow::Type::VecDrag, &TableRow::buildVecDrag },
			{ TableRow::Type::DragRange, &TableRow::buildDragRange },
			{ TableRow::Type::SimpleText, &TableRow::buildSimpleText }
		};

TableRow::TableRow(Table &table, Window *window, const char *rowName)
	:	_table{table},
		_window{window},
		_rowName{rowName} {
}

bool	TableRow::build(void) {
	return ((this->*_buildFunctions.at(_type))());
}

bool	TableRow::buildVecDrag(void) {
	if (!_start)
		return (false);
	size_t	sRange = static_cast<size_t>(_range);
	fillVec(_valueNames, sRange);
	fillVec(_mins, sRange);
	fillVec(_maxs, sRange);
	fillVec(_speeds, sRange);

	bool	changed = false;
	ImGui::PushID(_rowName);

	_table.newRow(_rowName);
	for (uint32_t i = 0; i < _range; i++) {
		_table.setNextCell(_valueNames[i], [&]{
			changed |= DragFloat(_window->getWindow(), _start + i)
							.setSpeed(_speeds[i])
							.setMin(_mins[i])
							.setMax(_maxs[i])
							.build();
			}
		);
	}

	ImGui::PopID();
	return (changed);
}

bool	TableRow::buildDragRange(void) {
	if (!_start)
		return (false);
	size_t	sRange = static_cast<size_t>(2);
	fillVec(_valueNames, sRange);
	fillVec(_mins, sRange);
	fillVec(_maxs, sRange);
	fillVec(_speeds, sRange);

	bool	changed = false;
	ImGui::PushID(_rowName);

	_table.newRow(_rowName);
	_table.setNextCell(_valueNames[0], [&]{
		changed |= DragFloat(_window->getWindow(), _start)
						.setSpeed(_speeds[0])
						.setMin(_mins[0])
						.setMax(*(_start + 1))
						.build();
		}
	);
	_table.setNextCell(_valueNames[1], [&]{
		changed |= DragFloat(_window->getWindow(), _start + 1)
						.setSpeed(_speeds[1])
						.setMin(*(_start))
						.setMax(_maxs[1])
						.build();
		}
	);
	ImGui::PopID();
	return (changed);
}

bool	TableRow::buildSimpleText(void) {
	if (!_start)
		return (false);
	size_t	sRange = static_cast<size_t>(_range);
	fillVec(_valueNames, sRange);
	fillVec(_fmts, sRange);

	ImGui::PushID(_rowName);

	_table.newRow(_rowName);
	for (uint32_t i = 0; i < _range; i++) {
		_table.setNextCell(_valueNames[i], [&]{
				ImGui::AlignTextToFramePadding();
				ImGui::Text(_fmts[i], *(_start + i));
			}
		);
	}

	ImGui::PopID();
	return false;
}

}
