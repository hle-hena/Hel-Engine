/* *************************************************************************	*/
/*																																						*/
/*																																						*/
/*	File: StyleEditor.cpp																										 */
/*	Project: Hel Engine																											 */
/*	Created: 2026/03/17 16:33:57 by hle-hena																	*/
/*																																						*/
/*  Last Modified: 2026/03/20 17:54:47                                        */
/*						 By: hle-hena																									 */
/*																																						*/
/*		-----																																	 */
/*																																						*/
/*	Copyright (c) 2026 hle-hena																							 */
/*																																						*/
/* *************************************************************************	*/

#include "platform/ui/StyleEditor.hpp"
#include "api/ImGui/imgui.h"

namespace	hel::sys {

std::vector<std::pair<const char *, ImVec4>>	StyleEditor::_baseColors = {
	{"Primary", ImVec4(0.f, 0.f, 0.f, 1.f)},
	{"Secondary", ImVec4(0.f, 0.f, 0.f, 1.f)},
	{"Tertiary", ImVec4(0.f, 0.f, 0.f, 1.f)},
	{"Shadow", ImVec4(0.f, 0.f, 0.f, 1.f)},
	{"Highlight", ImVec4(0.f, 0.f, 0.f, 1.f)}
};

expected<void, std::string>	StyleEditor::onInit(void) {
	using R = ReferenceColors;

	_colors = {
		{ ImGuiCol_WindowBg,				{ R::Secondary,	{}, 0.95f } },
		{ ImGuiCol_ChildBg,					{ R::Secondary,	{}, 0.50f } },
		{ ImGuiCol_PopupBg,					{ R::Secondary,	{}, 0.98f } },
		{ ImGuiCol_FrameBg,					{ R::Shadow,	{}, 0.30f } },
		{ ImGuiCol_FrameBgHovered,			{ R::Primary,	{}, 0.25f } },
		{ ImGuiCol_FrameBgActive,			{ R::Primary,	{}, 0.50f } },
		{ ImGuiCol_TitleBg,					{ R::Secondary,	{}, 1.00f } },
		{ ImGuiCol_TitleBgActive,			{ R::Primary,	{}, 0.70f } },
		{ ImGuiCol_MenuBarBg,				{ R::Secondary,	{}, 1.00f } },
		{ ImGuiCol_ScrollbarBg,				{ R::Shadow,	{}, 0.15f } },
		{ ImGuiCol_ScrollbarGrab,			{ R::Primary,	{}, 0.40f } },
		{ ImGuiCol_ScrollbarGrabHovered,	{ R::Primary,	{}, 0.65f } },
		{ ImGuiCol_ScrollbarGrabActive,		{ R::Primary,	{}, 0.90f } },
		{ ImGuiCol_CheckMark,				{ R::Primary,	{}, 1.00f } },
		{ ImGuiCol_SliderGrab,				{ R::Primary,	{}, 0.70f } },
		{ ImGuiCol_SliderGrabActive,		{ R::Primary,	{}, 1.00f } },
		{ ImGuiCol_Button,					{ R::Primary,	{}, 0.35f } },
		{ ImGuiCol_ButtonHovered,			{ R::Primary,	{}, 0.65f } },
		{ ImGuiCol_ButtonActive,			{ R::Primary,	{}, 1.00f } },
		{ ImGuiCol_Header,					{ R::Primary,	{}, 0.30f } },
		{ ImGuiCol_HeaderHovered,			{ R::Primary,	{}, 0.55f } },
		{ ImGuiCol_HeaderActive,			{ R::Primary,	{}, 0.80f } },
		{ ImGuiCol_Separator,				{ R::Shadow,	{}, 0.25f } },
		{ ImGuiCol_ResizeGrip,				{ R::Primary,	{}, 0.20f } },
		{ ImGuiCol_ResizeGripHovered,		{ R::Primary,	{}, 0.60f } },
		{ ImGuiCol_ResizeGripActive,		{ R::Primary,	{}, 0.95f } },
		{ ImGuiCol_Tab,						{ R::Secondary,	{}, 0.80f } },
		{ ImGuiCol_TabHovered,				{ R::Primary,	{}, 0.70f } },
		{ ImGuiCol_TabActive,				{ R::Primary,	{}, 0.50f } },
		{ ImGuiCol_DragDropTarget,			{ R::Primary,	{}, 0.90f } },
		{ ImGuiCol_NavHighlight,			{ R::Primary,	{}, 1.00f } },
		{ ImGuiCol_Text,					{ R::Highlight,	{}, 1.00f } },
		{ ImGuiCol_TextDisabled,			{ R::Highlight,	{}, 0.35f } },
		{ ImGuiCol_Border,					{ R::Shadow,	{}, 0.40f } },
		{ ImGuiCol_BorderShadow,			{ R::Shadow,	{}, 0.00f } }
	};
	return {};
}

ImVec4	StyleEditor::resolveColor(const Color &col) const {
	ImVec4	base;
	if (col.override) {
		base = col.color;
	} else {
		switch (col.reference) {
			case ReferenceColors::Primary:		base = _primaryColor;	break;
			case ReferenceColors::Secondary:	base = _secondaryColor;	break;
			case ReferenceColors::Shadow:		base = _shadowColor;	break;
			case ReferenceColors::Highlight:	base = _highlightColor;	break;
		}
	}
	return { base.x, base.y, base.z, col.alpha };
}

void	StyleEditor::applyPalette(void) {
	auto	&style = ImGui::GetStyle();
	for (auto &[colType, col] : _colors)
		style.Colors[colType] = resolveColor(col);
}

void	StyleEditor::renderColorRow(const char *name, Color &col) {
	ImGui::PushID(name);

	bool	changed = false;
	changed |= ImGui::Checkbox("##ov", &col.override);
	ImGui::SameLine();

	if (col.override) {
		changed |= ImGui::ColorEdit3(name, (float*)&col.color, ImGuiColorEditFlags_NoBorder);
	} else {
		const char	*refNames[] = { "Primary", "Secondary", "Shadow", "Highlight" };
		int ref = static_cast<int>(col.reference);
		ImGui::SetNextItemWidth(120.f);
		if (ImGui::Combo(name, &ref, refNames, 4)) {
			col.reference = static_cast<ReferenceColors>(ref);
			changed = true;
		}
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.f);
	changed |= ImGui::SliderFloat("##a", &col.alpha, 0.f, 1.f, "a=%.2f");

	if (changed)
		applyPalette();

	ImGui::PopID();
}

bool	StyleEditor::colorPicker(const char *label, ImVec4 &color) {
	ImGui::SeparatorText(label);

	auto	avail = ImGui::GetContentRegionAvail();
	float	pickerWidth = avail.x * 0.8f;
	ImGui::SetCursorPosX((avail.x - pickerWidth) * 0.5f);
	ImGui::SetNextItemWidth(pickerWidth);
	ImGui::ColorPicker3(label, &color.x, ImGuiColorEditFlags_DisplayRGB |
										ImGuiColorEditFlags_DisplayHSV |
										ImGuiColorEditFlags_DisplayHex |
										ImGuiColorEditFlags_PickerHueWheel |
										ImGuiColorEditFlags_NoSidePreview |
										ImGuiColorEditFlags_NoLabel |
										ImGuiColorEditFlags_NoSmallPreview);
}

void	StyleEditor::addColorTab(int index, const char *label, const ImVec4 &color) {
	bool	isSelected = (index == _colorTabSelected);
	if (isSelected)	{ ImGui::PushStyleColor(ImGuiCol_Header,
			ImGui::GetStyleColorVec4(ImGuiCol_TabSelected)); }
	ImGui::PushID(index);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.f, 0.f});
	if (ImGui::Selectable("##tabs", isSelected, 0, tabSize))
		_colorTabSelected = index;
	ImGui::PopStyleVar();
	if (ImGui::IsItemHovered())	{ ImGui::SetTooltip(label); }

	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetItemRectMin() + tabPadding,
		ImGui::GetItemRectMax() - tabPadding,
		ImGui::ColorConvertFloat4ToU32(color),
		3.f
	);

	ImGui::PopID();
	if (isSelected)
		ImGui::PopStyleColor();
}

bool	StyleEditor::baseColorEditor(void) {
	static constexpr ImVec2	tabSize = {32.f, 32.f};
	static constexpr ImVec2	tabPadding = {2.f, 3.f};

	ImVec2	childSize = {tabSize.x, tabSize.y * _baseColors.size()};
	ImGui::BeginChild("ColorTabSelection", childSize);
	for (int i = 0; i < _baseColors.size(); i++)
		addColorTab(i, _baseColors[i].first, _baseColors[i].second);
	ImGui::EndChild();

	ImGui::SameLine(0.f, 7.f);

	ImVec2 tabMax = ImGui::GetItemRectMax();
	ImVec2 tabMin = ImGui::GetItemRectMin();
	ImGui::GetWindowDrawList()->AddRectFilled(
		{tabMax.x + 4.f, tabMin.y},
		{tabMax.x + 7.f, tabMin.y + childSize.y},
		ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TabSelected)));

	ImGui::BeginChild("ColorPick", {0.f, 0.f}, ImGuiChildFlags_AutoResizeY);
	colorPicker(_baseColors[_colorTabSelected].first, _baseColors[_colorTabSelected].second);
	ImGui::EndChild();
}

void	StyleEditor::render(Window *window, const ImVec2 &size) {
	baseColorEditor();
	ImGui::Separator();



	ImGui::Dummy({0.f, 1000.f});

	ImGuiColorEditFlags	pickerFlags = ImGuiColorEditFlags_DisplayRGB |
								ImGuiColorEditFlags_DisplayHSV |
								ImGuiColorEditFlags_DisplayHex |
								ImGuiColorEditFlags_PickerHueBar;
	if (ImGui::BeginTabBar("##Colors_tab")) {
		if (ImGui::BeginTabItem("Primary")) {
			ImGui::ColorPicker3("Primary", &_primaryColor.x, pickerFlags);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Secondary")) {
			ImGui::ColorPicker3("Secondary", &_secondaryColor.x, pickerFlags);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Shadow")) {
			ImGui::ColorPicker3("Shadow", &_shadowColor.x, pickerFlags);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Highlight")) {
			ImGui::ColorPicker3("Highlight", &_highlightColor.x, pickerFlags);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	bool	paletteChanged = false;
	if (paletteChanged)
		applyPalette();

	ImGui::Separator();

	auto section = [&](const char *title, std::initializer_list<ImGuiCol_> cols) {
		if (!ImGui::CollapsingHeader(title)) return;
		for (auto col : cols)
			if (auto it = _colors.find(col); it != _colors.end())
				renderColorRow(ImGui::GetStyleColorName(col), it->second);
	};

	section("Backgrounds",	{ ImGuiCol_WindowBg, ImGuiCol_ChildBg, ImGuiCol_PopupBg, ImGuiCol_MenuBarBg });
	section("Frames",		{ ImGuiCol_FrameBg, ImGuiCol_FrameBgHovered, ImGuiCol_FrameBgActive });
	section("Title",		{ ImGuiCol_TitleBg, ImGuiCol_TitleBgActive });
	section("Buttons",		{ ImGuiCol_Button, ImGuiCol_ButtonHovered, ImGuiCol_ButtonActive });
	section("Headers",		{ ImGuiCol_Header, ImGuiCol_HeaderHovered, ImGuiCol_HeaderActive });
	section("Scrollbar",	{ ImGuiCol_ScrollbarBg, ImGuiCol_ScrollbarGrab, ImGuiCol_ScrollbarGrabHovered, ImGuiCol_ScrollbarGrabActive });
	section("Tabs",			{ ImGuiCol_Tab, ImGuiCol_TabHovered, ImGuiCol_TabActive });
	section("Text",			{ ImGuiCol_Text, ImGuiCol_TextDisabled });
	section("Misc",			{ ImGuiCol_Separator, ImGuiCol_Border, ImGuiCol_BorderShadow,
								ImGuiCol_CheckMark, ImGuiCol_SliderGrab, ImGuiCol_SliderGrabActive,
								ImGuiCol_ResizeGrip, ImGuiCol_ResizeGripHovered, ImGuiCol_ResizeGripActive,
								ImGuiCol_DragDropTarget, ImGuiCol_NavHighlight });
}

}
