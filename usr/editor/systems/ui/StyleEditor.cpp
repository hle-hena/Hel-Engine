/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: StyleEditor.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/18 11:20:37 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/23 10:22:25                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "systems/ui/StyleEditor.hpp"
#include "systems/ui/UIHelper.hpp"

#include <cmath>
#include <fstream>

namespace	hel::sys {

std::map<std::string, ImVec4>	StyleEditor::_baseColors = {};
std::map<ImGuiCol_, StyleEditor::Color>	StyleEditor::_colors = {};
std::vector<std::string>	StyleEditor::_baseColorsLabel = {
	"primary", "secondary", "tertiary", "shadow", "highlight"
};

StyleEditor::~StyleEditor(void) {
}

void	StyleEditor::saveToFile(const std::string &path) {
	std::ofstream	file(path);
	file << serialize().dump(2);
}

bool	StyleEditor::loadFromFile(const std::string &path) {
	std::ifstream	file(path);
	if (file.is_open()) {
		nlohmann::json	src;
		file >> src;
		deserialize(src);
		return (true);
	}

	if (_baseColors.empty()) {
		_baseColors = {
			{"highlight", {1.000f, 1.000f, 1.000f, 1.000f}},
			{"primary", {0.484f, 0.892f, 0.958f, 1.000f}},
			{"secondary", {0.088f, 0.092f, 0.153f, 1.000f}},
			{"shadow", {0.000f, 0.000f, 0.000f, 1.000f}},
			{"tertiary", {0.207f, 0.938f, 0.793f, 1.000f}},
		};
	}

	if (_colors.empty()) {
		_colors = {
			{ ImGuiCol_Text, { "highlight", {}, 1.00f } },
			{ ImGuiCol_TextDisabled, { "highlight", {}, 0.35f } },
			{ ImGuiCol_WindowBg, { "secondary", {}, 0.95f } },
			{ ImGuiCol_ChildBg, { "secondary", {}, 0.50f } },
			{ ImGuiCol_PopupBg, { "secondary", {}, 0.98f } },
			{ ImGuiCol_Border, { "shadow", {}, 0.40f } },
			{ ImGuiCol_BorderShadow, { "shadow", {}, 0.00f } },
			{ ImGuiCol_FrameBg, { "shadow", {}, 0.30f } },
			{ ImGuiCol_FrameBgHovered, { "primary", {}, 0.25f } },
			{ ImGuiCol_FrameBgActive, { "primary", {}, 0.50f } },
			{ ImGuiCol_TitleBg, { "secondary", {}, 1.00f } },
			{ ImGuiCol_TitleBgActive, { "primary", {}, 0.70f } },
			{ ImGuiCol_TitleBgCollapsed, { "primary", {}, 1.00f } },
			{ ImGuiCol_MenuBarBg, { "secondary", {}, 1.00f } },
			{ ImGuiCol_ScrollbarBg, { "shadow", {}, 0.15f } },
			{ ImGuiCol_ScrollbarGrab, { "primary", {}, 0.40f } },
			{ ImGuiCol_ScrollbarGrabHovered, { "primary", {}, 0.65f } },
			{ ImGuiCol_ScrollbarGrabActive, { "primary", {}, 0.90f } },
			{ ImGuiCol_CheckMark, { "primary", {}, 1.00f } },
			{ ImGuiCol_SliderGrab, { "primary", {}, 0.70f } },
			{ ImGuiCol_SliderGrabActive, { "primary", {}, 1.00f } },
			{ ImGuiCol_Button, { "primary", {}, 0.35f } },
			{ ImGuiCol_ButtonHovered, { "primary", {}, 0.65f } },
			{ ImGuiCol_ButtonActive, { "primary", {}, 1.00f } },
			{ ImGuiCol_Header, { "primary", {}, 0.30f } },
			{ ImGuiCol_HeaderHovered, { "primary", {}, 0.55f } },
			{ ImGuiCol_HeaderActive, { "primary", {}, 0.80f } },
			{ ImGuiCol_Separator, { "shadow", {}, 0.25f } },
			{ ImGuiCol_SeparatorHovered, { "primary", {}, 1.00f } },
			{ ImGuiCol_SeparatorActive, { "primary", {}, 1.00f } },
			{ ImGuiCol_ResizeGrip, { "primary", {}, 0.20f } },
			{ ImGuiCol_ResizeGripHovered, { "primary", {}, 0.60f } },
			{ ImGuiCol_ResizeGripActive, { "primary", {}, 0.95f } },
			{ ImGuiCol_InputTextCursor, { "primary", {}, 1.00f } },
			{ ImGuiCol_TabHovered, { "primary", {}, 0.70f } },
			{ ImGuiCol_Tab, { "secondary", {}, 0.80f } },
			{ ImGuiCol_TabSelected, { "primary", {}, 0.50f } },
			{ ImGuiCol_TabSelectedOverline, { "primary", {}, 1.00f } },
			{ ImGuiCol_TabDimmed, { "primary", {}, 1.00f } },
			{ ImGuiCol_TabDimmedSelected, { "primary", {}, 1.00f } },
			{ ImGuiCol_TabDimmedSelectedOverline, { "primary", {}, 1.00f } },
			{ ImGuiCol_DockingPreview, { "primary", {}, 1.00f } },
			{ ImGuiCol_DockingEmptyBg, { "primary", {}, 1.00f } },
			{ ImGuiCol_TextLink, { "primary", {}, 1.00f } },
			{ ImGuiCol_TextSelectedBg, { "primary", {}, 1.00f } },
			{ ImGuiCol_DragDropTarget, { "tertiary", {}, 1.00f } },
			{ ImGuiCol_DragDropTargetBg, { "tertiary", {}, 0.45f } },
			{ ImGuiCol_UnsavedMarker, { "primary", {}, 1.00f } },
			{ ImGuiCol_NavCursor, { "primary", {}, 1.00f } },
			{ ImGuiCol_NavWindowingHighlight, { "primary", {}, 1.00f } },
			{ ImGuiCol_NavWindowingDimBg, { "primary", {}, 1.00f } },
			{ ImGuiCol_ModalWindowDimBg, { "primary", {}, 1.00f } },
		};
	}
	return (true);
}

nlohmann::json	StyleEditor::serialize(void) {
	nlohmann::json	dst;
	for (auto &name: _baseColorsLabel) {
		auto	col = _baseColors[name];
		dst["baseColors"][name] = {col.x, col.y, col.z, col.w};
	}
    for (auto &[colType, col]: _colors) {
        nlohmann::json	c;
        c["reference"] = col.reference;
        c["alpha"] = col.alpha;
        c["override"] = col.override;
        c["color"] = {col.color.x, col.color.y, col.color.z, col.color.w};
        dst["colors"][std::to_string(colType)] = c;
    }
	return (dst);
}

void	StyleEditor::deserialize(const nlohmann::json &src) {
	if (src.contains("baseColors")) {
		for (auto &[name, arr]: src["baseColors"].items())
			_baseColors[name] = {arr[0], arr[1], arr[2], arr[3]};
	}
	if (src.contains("colors")) {
		for (auto &[key, color]: src["colors"].items()) {
			ImGuiCol_ colType = static_cast<ImGuiCol_>(std::stoi(key));
			_colors[colType] = {color["reference"], {}, color["alpha"],
				color["override"]};
			_colors[colType].color = {
				color["color"][0],
				color["color"][1],
				color["color"][2],
				color["color"][3]
			};
		}
	}
}

expected<void>	StyleEditor::onInit(void) {
	loadFromFile("currentStyle.json");
	return {};
}

ImVec4	StyleEditor::resolveColor(const Color &col) {
	auto	base = col.override ? col.color : _baseColors[col.reference];
	return { base.x, base.y, base.z, col.alpha };
}

void	StyleEditor::applyPalette(void) {
	auto	&style = ImGui::GetStyle();
	for (auto &[colType, col] : _colors)
		style.Colors[colType] = resolveColor(col);
}

void	StyleEditor::applyPalette(ImGuiCol col, ImVec4 colValue) {
	ImGui::GetStyle().Colors[col] = colValue;
}

bool	StyleEditor::colorPicker(const std::string &colorName, ImVec4 &color) {
	ImGui::SeparatorText(("Modify the " + colorName + " color !").c_str());

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
	return (ImGui::ColorPicker3(colorName.c_str(), &color.x, pickerFlags));
}

bool	StyleEditor::basePopup(Color &color) {
	color.override = false;
	float		nbColors = static_cast<float>(_baseColorsLabel.size());
	float		nbColumns = round(sqrt(nbColors));
	if (auto	table = Table("ChoseBaseColor")) {
		ImVec2	avail = ImGui::GetContentRegionAvail() -
						ImVec2(0.f, ImGui::GetFrameHeight() * 2);
		ImVec2	tabSize = {avail.x / nbColumns, avail.y / std::ceil(nbColors / nbColumns)};
		ImVec2	tabPadding = {tabSize.x * 0.025f, tabSize.y * 0.05f};
		size_t	nbColors_t = static_cast<size_t>(nbColors);
		size_t	nbColumns_t = static_cast<size_t>(nbColumns);
		for (size_t row = 0; row < nbColors_t; row += nbColumns_t) {
			size_t	columns = std::min(nbColumns_t, nbColors_t - row);
			Table::ColumnSizing	sizing(columns, Table::WStretch);
	
			table.newRow(sizing);
			for (size_t column = 0; column < columns; column++) {
				table.setNextCell([&]{
					float	cellWidth = ImGui::GetContentRegionAvail().x;
					float	itemWidth = tabSize.x;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cellWidth - itemWidth) * 0.5f);
					const auto	&label = _baseColorsLabel[row + column];
					if (colorSelectable(label, _baseColors[label],
									{"Click to change to the %s color !",
									label == color.reference,
									tabSize, tabPadding}))
						color.reference = label;
				});
			}
		}
	}
	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
	bool	clicked = ImGui::Button("Apply", ImGui::GetContentRegionAvail());
	ImGui::PopStyleColor();
	if (clicked)	{ ImGui::CloseCurrentPopup(); }
	return (clicked);
}

bool	StyleEditor::overridePopup(Color &color) {
	color.override = true;
	ImGui::Dummy({0.f, 10.f});
	colorPicker("override", color.color);
	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
	bool	clicked = ImGui::Button("Apply", ImGui::GetContentRegionAvail());
	ImGui::PopStyleColor();
	if (clicked)	{ ImGui::CloseCurrentPopup(); }
	return (clicked);
}

bool	StyleEditor::changeColorPopup(Color &color) {
	bool	changed = false;

	ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 0.3f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {4.f, 4.f});
	ImGui::SetNextWindowSize({300.f, 400.f}, ImGuiCond_Appearing);
	if (ImGui::BeginPopup("CHANGE_COLOR")) {
		ImGui::BeginChild("ColorChangePopup");
		ImGui::SeparatorText("Modify the color of the element");
		if (ImGui::BeginTabBar("ChangeColorTabBar", ImGuiTabBarFlags_NoTabListScrollingButtons)) {
			if (ImGui::BeginTabItem("Base colors")) {
				changed |= basePopup(color);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Override color")) {
				changed |= overridePopup(color);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar(2);
	return (changed);
}

bool	StyleEditor::renderColorRow(const char *name, Color &col) {
	auto	table = Table("ColorRow");

	bool	changed = false;
	if (!table.newRow(name, {Table::WFixed, Table::WFixed, Table::WFixed,
						Table::WStretch}))
		return (changed);
	ImGui::PushID(name);

	table.setNextCell("Alpha:", [&]{
		changed |= Knob(&col.alpha)
					.setLabel("##alphaColorKnob")
					.setMin(0.f)
					.setMax(1.f)
					.build();
	});
	table.setNextCell([&]{
		changed |= changeColorPopup(col);
		if (ImGui::Button("Change the color"))
			ImGui::OpenPopup("CHANGE_COLOR");
	});
	ImGui::PopID();
	return (changed);
}

void	StyleEditor::showColorSection(const char *sectionName,
									const ColorList &colors) {
	if (!ImGui::CollapsingHeader(sectionName))	{ return ; }
	for (auto color: colors) {
		auto	it = _colors.find(color);
		if (it == _colors.end())
			_colors[color] = {"primary", {}, 1.f};
		if (renderColorRow(ImGui::GetStyleColorName(color), it->second))
			applyPalette(color, resolveColor(it->second));
	}
}

bool	StyleEditor::colorSelectable(const std::string &colorName,
									const ImVec4 &color,
									const ColorSelectableStyle &style) {
	if (style.isSelected)	{ ImGui::PushStyleColor(ImGuiCol_Header,
			ImGui::GetStyleColorVec4(ImGuiCol_TabSelected)); }
	ImGui::PushID(colorName.c_str());

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.f, 0.f});

	bool	hasBeenSelected = ImGui::Selectable("##tabs", style.isSelected,
														0, style.tabSize);
	ImGui::PopStyleVar();
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(style.format, colorName.c_str());

	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetItemRectMin() + style.tabPadding,
		ImGui::GetItemRectMax() - style.tabPadding,
		ImGui::ColorConvertFloat4ToU32(color),
		std::max(style.tabSize.x, style.tabSize.y) * 0.1f
	);

	ImGui::PopID();
	if (style.isSelected)
		ImGui::PopStyleColor();
	return (hasBeenSelected);
}

void	StyleEditor::baseColorEditor(void) {
	ImVec2	childSize = {_tabSize.x,
		_tabSize.y * static_cast<float>(_baseColorsLabel.size())};
	ImGui::BeginChild("ColorTabSelection", childSize);
	for (auto &colorName: _baseColorsLabel) {
		if (colorSelectable(colorName, _baseColors[colorName],
						{"Click to change the %s color !",
						label == _colorTabSelected,
						_tabSize, _tabPadding}))
			_colorTabSelected = colorName;
	}
	ImGui::EndChild();

	ImGui::SameLine(0.f, 7.f);

	ImVec2 tabMax = ImGui::GetItemRectMax();
	ImVec2 tabMin = ImGui::GetItemRectMin();
	ImGui::GetWindowDrawList()->AddRectFilled(
		{tabMax.x + 4.f, tabMin.y},
		{tabMax.x + 7.f, tabMin.y + childSize.y},
		ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TabSelected)));

	ImGui::BeginChild("ColorPick", {0.f, 0.f}, ImGuiChildFlags_AutoResizeY);
	if (colorPicker(_colorTabSelected, _baseColors[_colorTabSelected]))
		applyPalette();
	ImGui::EndChild();
}

void	StyleEditor::render(const FrameContext &, const ImVec2 &) {
	baseColorEditor();
	ImGui::Separator();

	showColorSection("Text", _textColors);
	showColorSection("Windows & Frames", _windowColors);
	showColorSection("Widgets", _widgetColors);
	showColorSection("Tabs", _tabColors);
	showColorSection("Tables & Plots", _windowColors);
	showColorSection("Misc", _miscColors);
}

}
