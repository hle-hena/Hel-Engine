/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: StyleEditor.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/18 11:20:37 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/20 20:32:16                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/StyleEditor.hpp"
#include "api/ImGui/imgui.h"
#include "platform/ui/UIHelper.hpp"

#include <cmath>

namespace	hel::sys {

std::map<std::string, ImVec4>	StyleEditor::_baseColors = {
	{"primary", ImVec4(0.f, 0.f, 0.f, 1.f)},
	{"secondary", ImVec4(0.f, 0.f, 0.f, 1.f)},
	{"tertiary", ImVec4(0.f, 0.f, 0.f, 1.f)},
	{"shadow", ImVec4(0.f, 0.f, 0.f, 1.f)},
	{"highlight", ImVec4(0.f, 0.f, 0.f, 1.f)}
};
std::vector<std::string>	StyleEditor::_baseColorsLabel = {
	"primary", "secondary", "tertiary", "shadow", "highlight"
};

expected<void, std::string>	StyleEditor::onInit(void) {

	_colors = {
		{ ImGuiCol_WindowBg,				{ "secondary",	{}, 0.95f } },
		{ ImGuiCol_ChildBg,					{ "secondary",	{}, 0.50f } },
		{ ImGuiCol_PopupBg,					{ "secondary",	{}, 0.98f } },
		{ ImGuiCol_FrameBg,					{ "shadow",		{}, 0.30f } },
		{ ImGuiCol_FrameBgHovered,			{ "primary",	{}, 0.25f } },
		{ ImGuiCol_FrameBgActive,			{ "primary",	{}, 0.50f } },
		{ ImGuiCol_TitleBg,					{ "secondary",	{}, 1.00f } },
		{ ImGuiCol_TitleBgActive,			{ "primary",	{}, 0.70f } },
		{ ImGuiCol_MenuBarBg,				{ "secondary",	{}, 1.00f } },
		{ ImGuiCol_ScrollbarBg,				{ "shadow",		{}, 0.15f } },
		{ ImGuiCol_ScrollbarGrab,			{ "primary",	{}, 0.40f } },
		{ ImGuiCol_ScrollbarGrabHovered,	{ "primary",	{}, 0.65f } },
		{ ImGuiCol_ScrollbarGrabActive,		{ "primary",	{}, 0.90f } },
		{ ImGuiCol_CheckMark,				{ "primary",	{}, 1.00f } },
		{ ImGuiCol_SliderGrab,				{ "primary",	{}, 0.70f } },
		{ ImGuiCol_SliderGrabActive,		{ "primary",	{}, 1.00f } },
		{ ImGuiCol_Button,					{ "primary",	{}, 0.35f } },
		{ ImGuiCol_ButtonHovered,			{ "primary",	{}, 0.65f } },
		{ ImGuiCol_ButtonActive,			{ "primary",	{}, 1.00f } },
		{ ImGuiCol_Header,					{ "primary",	{}, 0.30f } },
		{ ImGuiCol_HeaderHovered,			{ "primary",	{}, 0.55f } },
		{ ImGuiCol_HeaderActive,			{ "primary",	{}, 0.80f } },
		{ ImGuiCol_Separator,				{ "shadow",		{}, 0.25f } },
		{ ImGuiCol_ResizeGrip,				{ "primary",	{}, 0.20f } },
		{ ImGuiCol_ResizeGripHovered,		{ "primary",	{}, 0.60f } },
		{ ImGuiCol_ResizeGripActive,		{ "primary",	{}, 0.95f } },
		{ ImGuiCol_Tab,						{ "secondary",	{}, 0.80f } },
		{ ImGuiCol_TabHovered,				{ "primary",	{}, 0.70f } },
		{ ImGuiCol_TabActive,				{ "primary",	{}, 0.50f } },
		{ ImGuiCol_DragDropTarget,			{ "primary",	{}, 0.90f } },
		{ ImGuiCol_NavHighlight,			{ "primary",	{}, 1.00f } },
		{ ImGuiCol_Text,					{ "highlight",	{}, 1.00f } },
		{ ImGuiCol_TextDisabled,			{ "highlight",	{}, 0.35f } },
		{ ImGuiCol_Border,					{ "shadow",		{}, 0.40f } },
		{ ImGuiCol_BorderShadow,			{ "shadow",		{}, 0.00f } }
	};
	return {};
}

ImVec4	StyleEditor::resolveColor(const Color &col) const {
	auto	base = col.override ? col.color : _baseColors[col.reference];
	return { base.x, base.y, base.z, col.alpha };
}

void	StyleEditor::applyPalette(void) {
	auto	&style = ImGui::GetStyle();
	for (auto &[colType, col] : _colors)
		style.Colors[colType] = resolveColor(col);
}

bool	StyleEditor::colorPicker(const std::string &label, ImVec4 &color) {
	ImGui::SeparatorText(("Modify the " + label + " color !").c_str());

	auto	avail = ImGui::GetContentRegionAvail();
	float	pickerWidth = avail.x * 0.8f;
	ImGui::SetCursorPosX((avail.x - pickerWidth) * 0.5f);
	ImGui::SetNextItemWidth(pickerWidth);
	ImGuiColorEditFlags	pickerFlags = ImGuiColorEditFlags_DisplayRGB |
									ImGuiColorEditFlags_DisplayHSV |
									ImGuiColorEditFlags_DisplayHex |
									ImGuiColorEditFlags_PickerHueWheel |
									ImGuiColorEditFlags_NoSidePreview |
									ImGuiColorEditFlags_NoLabel |
									ImGuiColorEditFlags_NoSmallPreview;
	ImGui::ColorPicker3(label.c_str(), &color.x, pickerFlags);
}

void	StyleEditor::renderColorRow(const char *name, Color &col) {
	ImGui::PushID(name);

	bool	changed = false;
	changed |= ImGui::Checkbox("##override", &col.override);
	ImGui::SameLine();

	if (col.override) {
		changed |= ImGui::ColorEdit3(name, (float*)&col.color, ImGuiColorEditFlags_NoBorder);//Change.
	} else {
		auto	retrieveStr = [](void *data, int i){
			return ((*static_cast<std::vector<std::string>*>(data))[i].c_str());
		};
		int		ref = std::distance(_baseColorsLabel.begin(), std::find(
					_baseColorsLabel.begin(), _baseColorsLabel.end(), col.reference));
		ImGui::SetNextItemWidth(120.f);

		if (ImGui::Combo(name, &ref, retrieveStr, &_baseColorsLabel,
						_baseColorsLabel.size())) {
			col.reference = _baseColorsLabel[ref];
			changed = true;
		}
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.f);
	changed |= Knob(&col.alpha)
				.setLabel("##alphaColorKnob")
				.setMin(0.f)
				.setMax(1.f)
				.build();

	// if (changed)
	// 	applyPalette();

	ImGui::PopID();
}

void	StyleEditor::showColorSection(const char *sectionName,
									const ColorList &colors) {
	if (!ImGui::CollapsingHeader(sectionName))	{ return ; }
	for (auto color: colors) {
		auto	it = _colors.find(color);
		if (it != _colors.end())
			renderColorRow(ImGui::GetStyleColorName(color), it->second);
	}
}

void	StyleEditor::addColorTab(const std::string &label,
								const ImVec4 &color) {
	bool	isSelected = (label == _colorTabSelected);
	if (isSelected)	{ ImGui::PushStyleColor(ImGuiCol_Header,
			ImGui::GetStyleColorVec4(ImGuiCol_TabSelected)); }
	ImGui::PushID(label.c_str());

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.f, 0.f});
	if (ImGui::Selectable("##tabs", isSelected, 0, _tabSize))
		_colorTabSelected = label;
	ImGui::PopStyleVar();
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(("Click to modify the " + label + " color").c_str());

	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetItemRectMin() + _tabPadding,
		ImGui::GetItemRectMax() - _tabPadding,
		ImGui::ColorConvertFloat4ToU32(color),
		3.f
	);

	ImGui::PopID();
	if (isSelected)
		ImGui::PopStyleColor();
}

bool	StyleEditor::baseColorEditor(void) {
	ImVec2	childSize = {_tabSize.x, _tabSize.y * _baseColors.size()};
	ImGui::BeginChild("ColorTabSelection", childSize);
	for (auto &label: _baseColorsLabel)
		addColorTab(label, _baseColors[label]);
	ImGui::EndChild();

	ImGui::SameLine(0.f, 7.f);

	ImVec2 tabMax = ImGui::GetItemRectMax();
	ImVec2 tabMin = ImGui::GetItemRectMin();
	ImGui::GetWindowDrawList()->AddRectFilled(
		{tabMax.x + 4.f, tabMin.y},
		{tabMax.x + 7.f, tabMin.y + childSize.y},
		ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TabSelected)));

	ImGui::BeginChild("ColorPick", {0.f, 0.f}, ImGuiChildFlags_AutoResizeY);
	colorPicker(_colorTabSelected, _baseColors[_colorTabSelected]);
	ImGui::EndChild();
}

void	StyleEditor::render(Window *window, const ImVec2 &size) {
	if (baseColorEditor())
		applyPalette();
	ImGui::Separator();

	showColorSection("Text", _textColors);
	showColorSection("Windows & Frames", _windowColors);
	showColorSection("Widgets", _widgetColors);
	showColorSection("Tabs", _tabColors);
	showColorSection("Tables & Plots", _windowColors);
	showColorSection("Misc", _miscColors);
}

}
