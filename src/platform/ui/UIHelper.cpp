/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/03 11:52:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 18:08:07                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/UIHelper.hpp"

#include <ui/ImGui/imgui_internal.h>
#include <ui/ImGui/imgui_stdlib.h>
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
						IsPositive(_dir) * ImGui::GetIO().MouseDelta.y / _normalizer :
						IsPositive(_dir) * ImGui::GetIO().MouseDelta.x / _normalizer;
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

Table::~Table(void) {
	if (_tableOpened) {
		ImGui::EndTable();
		_tableOpened = false;
	}
}

bool	Table::beginNewTable(ColumnSizing columnSizing) {
	if (_tableOpened)
		return (false);
	std::string	indexedName = std::string(_name) + "###Table" + std::to_string(_nbCol);
	if (ImGui::BeginTable(indexedName.c_str(), _nbCol, ImGuiTableFlags_SizingFixedFit)) {
		for (auto sizing: columnSizing)
			ImGui::TableSetupColumn(nullptr, sizing);
		_tableOpened = true;
		_nbTables++;
		return (true);
	}
	return (false);
}

void	Table::endTable(void) {
	if (_tableOpened) {
		ImGui::EndTable();
		_tableOpened = false;
	}
}

bool	Table::newRow(ColumnSizing columnSizing) {
	int	nbCol = columnSizing.size();
	if (nbCol != _nbCol || !_tableOpened) {
		_nbCol = nbCol;
		endTable();
		if (!beginNewTable(columnSizing))
			return (false);
	}
	ImGui::TableNextRow();
	return (true);
}

bool	Table::newRow(const char *rowName, ColumnSizing columnSizing) {
	int	nbCol = columnSizing.size();
	if (nbCol != _nbCol || !_tableOpened) {
		_nbCol = nbCol;
		endTable();
		if (!beginNewTable(columnSizing))
			return (false);
	}
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::AlignTextToFramePadding();
	ImGui::Text(rowName);
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(10.0f, 0.0f));
	return (true);
}



const std::unordered_map<TableRow::Type, TableRow::BuildFunc>
		TableRow::_buildFunctions = {
			{ TableRow::Type::VecDrag, &TableRow::buildVecDrag },
			{ TableRow::Type::DragRange, &TableRow::buildDragRange },
			{ TableRow::Type::SimpleText, &TableRow::buildSimpleText },
			{ TableRow::Type::InputText, &TableRow::buildInputText }
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
	if (!_startFloat)
		return (false);
	size_t	sRange = static_cast<size_t>(_range);
	fillVec(_valueNames, sRange);
	fillVec(_fmts, sRange);
	fillVec(_mins, sRange);
	fillVec(_maxs, sRange);
	fillVec(_speeds, sRange);

	bool	changed = false;
	Table::ColumnSizing	sizing = {Table::WFixed};
	for (int i = 0; i < sRange; i++) {
		if (_valueNames[i])
			sizing.push_back(Table::WFixed);
		sizing.push_back(Table::WStretch);
	}
	if (!_table.newRow(_rowName, sizing))
		return (false);
	ImGui::PushID(_rowName);

	for (uint32_t i = 0; i < _range; i++) {
		ImGui::PushID(i);
		_table.setNextCell(_valueNames[i], [&]{
				changed |= DragFloat(_window->getWindow(), _startFloat + i)
								.setSpeed(_speeds[i])
								.setMin(_mins[i])
								.setMax(_maxs[i])
								.setFormat(_fmts[i])
								.build();
			}
		);
		ImGui::PopID();
	}

	ImGui::PopID();
	return (changed);
}

bool	TableRow::buildDragRange(void) {
	if (!_startFloat)
		return (false);
	_range = 2;
	size_t	sRange = static_cast<size_t>(_range);
	fillVec(_valueNames, sRange);
	fillVec(_fmts, sRange);
	fillVec(_mins, sRange);
	fillVec(_maxs, sRange);
	fillVec(_speeds, sRange);
	_maxs[0] = *(_startFloat + 1);
	_mins[1] = *(_startFloat);

	bool	changed = false;
	Table::ColumnSizing	sizing = {Table::WFixed};
	for (int i = 0; i < sRange; i++) {
		if (_valueNames[i])
			sizing.push_back(Table::WFixed);
		sizing.push_back(Table::WStretch);
	}
	if (!_table.newRow(_rowName, sizing))
		return (false);
	ImGui::PushID(_rowName);

	for (uint32_t i = 0; i < _range; i++) {
		ImGui::PushID(i);
		_table.setNextCell(_valueNames[i], [&]{
				changed |= DragFloat(_window->getWindow(), _startFloat + i)
								.setSpeed(_speeds[i])
								.setMin(_mins[i])
								.setMax(_maxs[i])
								.setFormat(_fmts[i])
								.build();
			}
		);
		ImGui::PopID();
	}

	ImGui::PopID();
	return (changed);
}

bool	TableRow::buildSimpleText(void) {
	if (!_startFloat)
		return (false);
	size_t	sRange = static_cast<size_t>(_range);
	fillVec(_valueNames, sRange);
	fillVec(_fmts, sRange);

	Table::ColumnSizing	sizing = {Table::WFixed};
	for (int i = 0; i < sRange; i++) {
		if (_valueNames[i])
			sizing.push_back(Table::WFixed);
		sizing.push_back(Table::WStretch);
	}
	if (!_table.newRow(_rowName, sizing))
		return (false);
	ImGui::PushID(_rowName);

	for (uint32_t i = 0; i < _range; i++) {
		ImGui::PushID(i);
		_table.setNextCell(_valueNames[i], [&]{
				ImGui::AlignTextToFramePadding();
				ImGui::Text(_fmts[i], *(_startFloat + i));
			}
		);
		ImGui::PopID();
	}

	ImGui::PopID();
	return (false);
}

bool	TableRow::buildInputText(void) {
	if (!_startString)
		return (false);
	size_t	sRange = static_cast<size_t>(_range);
	fillVec(_valueNames, sRange);

	bool	changed = false;
	Table::ColumnSizing	sizing = {Table::WFixed};
	for (int i = 0; i < sRange; i++) {
		if (_valueNames[i])
			sizing.push_back(Table::WFixed);
		sizing.push_back(Table::WStretch);
	}
	if (!_table.newRow(_rowName, sizing))
		return (false);
	ImGui::PushID(_rowName);

	for (uint32_t i = 0; i < _range; i++) {
		ImGui::PushID(i);
		_table.setNextCell(_valueNames[i], [&]{
				changed |= ImGui::InputText("##", _startString + i);
			}
		);
		ImGui::PopID();
	}

	ImGui::PopID();
	return (changed);
}



DropTarget::DropTarget(const char *type)
	:	_type{type} {
	_startPos = ImGui::GetCursorScreenPos();
	_endPos = _startPos;
	_size = ImGui::GetContentRegionAvail();
	_setToEndPos = false;
}

DropTarget	&DropTarget::setEndPos(const ImVec2 &val) {
	_endPos = val;
	_setToEndPos = true;
	return (*this);
}

DropTarget	&DropTarget::addDummy(void) {
	ImGui::SetCursorScreenPos(_startPos);
	ImGui::Dummy(_size);
	if (_setToEndPos) {
		ImGui::SetCursorScreenPos(_endPos);
		ImGui::GetCurrentWindow()->DC.IsSetPos = false;
	}
	return (*this);
}



Button::Button(const char *label) {
	_label = label;
	_size = ImGui::GetContentRegionAvail();
	_pos = ImGui::GetCursorScreenPos();
}

Button	&Button::showOnHover(bool parentHover) {
	_hide = parentHover;
	_hide |= ImGui::IsMouseHoveringRect(_pos, {_pos.x + _size.x, _pos.y + _size.y});
	return (*this);
}

bool	Button::build() {
	if (!_hide)
		return (false);
	ImGui::SetCursorScreenPos(_pos);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0.f, 0.f});
	bool	ret = ImGui::Button(_label, _size);
	ImGui::PopStyleVar(2);
	if (_endPos)
		ImGui::SetCursorScreenPos(*_endPos);
	ImGui::GetCurrentWindow()->DC.IsSetPos = false;
	return (ret);
}



bool	Knob::build(void) {
	float	startAngle = IM_PI * 0.75f;
	float	endAngle = IM_PI * 2.25f;
	float	angleRange = endAngle - startAngle;

	bool	changed = false;
	ImVec2	center = ImGui::GetCursorScreenPos() + ImVec2(_radius, _radius);
	auto	*draw = ImGui::GetWindowDrawList();

	ImGui::InvisibleButton(_label, {_radius * 2.f, _radius * 2.f});

	if (ImGui::IsItemActive()) {
		ImVec2	mouse = ImGui::GetIO().MousePos;
		float	angle = atan2f(mouse.y - center.y, mouse.x - center.x);
		if (angle < 0.f)	{ angle += 2.f * IM_PI; }

		float	start = startAngle;
		float	end = endAngle;
		if (angle < end - 2.f * IM_PI)	{ angle += 2.f * IM_PI; }
		float	t = (angle - start) / angleRange;

		if (t < 0.f || t > 1.f) {
			float	distToStart = std::abs(angle - start);
			float	distToEnd = std::abs(angle - end);
			if (distToStart > IM_PI)
				distToStart = 2.f * IM_PI - distToStart;
			if (distToEnd > IM_PI)
				distToEnd = 2.f * IM_PI - distToEnd;
			t = (distToEnd < distToStart) ? 1.f : 0.f;
		}

		*_val = _min + t * (_max - _min);
		changed = true;
	}

	float	t = (*_val - _min) / (_max - _min);
	float	valueAngle = startAngle + t * angleRange;

	ImU32	bgColor = ImGui::ColorConvertFloat4ToU32(
		ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
	ImU32	fgColor = ImGui::ColorConvertFloat4ToU32(
		ImGui::GetStyleColorVec4(ImGuiCol_SliderGrabActive));

	draw->PathArcTo(center, _radius, startAngle, endAngle, 64);
	draw->PathStroke(bgColor, 0, _thickness);

	draw->PathArcTo(center, _radius, startAngle, valueAngle, 64);
	draw->PathStroke(fgColor, 0, _thickness);

	return (changed);
}



ColoredDummy::ColoredDummy(void) {
	_size = ImGui::GetContentRegionAvail();
	_pos = ImGui::GetCursorScreenPos();
}

void	ColoredDummy::build(void) {
	auto	draw = ImGui::GetForegroundDrawList();
	ImVec2	rectMax = {_pos.x + _size.x, _pos.y + _size.y};

	ImGui::SetCursorScreenPos(_pos);
	// ImGui::Dummy(_size);
	draw->AddRectFilled(_pos, rectMax, IM_COL32(255, 0, 0, 100));
	ImGui::SetCursorScreenPos(_endPos);
	ImGui::GetCurrentWindow()->DC.IsSetPos = false;
}


}
