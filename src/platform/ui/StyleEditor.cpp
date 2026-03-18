/* *************************************************************************	*/
/*																																						*/
/*																																						*/
/*	File: StyleEditor.cpp																										 */
/*	Project: Hel Engine																											 */
/*	Created: 2026/03/17 16:33:57 by hle-hena																	*/
/*																																						*/
/*  Last Modified: 2026/03/18 10:27:12                                        */
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

void	StyleEditor::render(Window *window, const ImVec2 &) {
	bool	paletteChanged = false;
	paletteChanged |= ImGui::ColorEdit3("Primary",	(float*)&_primaryColor);
	paletteChanged |= ImGui::ColorEdit3("Secondary",(float*)&_secondaryColor);
	paletteChanged |= ImGui::ColorEdit3("Shadow",	(float*)&_shadowColor);
	paletteChanged |= ImGui::ColorEdit3("Highlight",(float*)&_highlightColor);
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
