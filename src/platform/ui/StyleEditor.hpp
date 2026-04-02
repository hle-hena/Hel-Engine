/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: StyleEditor.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/17 16:32:44 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 18:03:20                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "platform/ui/Panel.hpp"


# include <map>
# include <utils/json.hpp>
# include <ui/ImGui/imgui.h>

namespace	hel::sys {

class	StyleEditor : public Panel<StyleEditor> {
	public:
		struct	Color {
			std::string		reference{"primary"};
			ImVec4			color{0.f, 0.f, 0.f, 1.f};
			float			alpha{1.f};
			bool			override{false};
		};

		static constexpr const char	*label = "UI Style";
		StyleEditor(void) = default;
		~StyleEditor(void);

		expected<void, std::string>	onInit(void) override;
		static void		applyPalette(void);
		static bool		loadFromFile(const std::string &path);

		void	render(Window *window, const ImVec2 &) override;

	private:
		void			saveToFile(const std::string &path);
		nlohmann::json	serialize(void) const;
		static void		deserialize(const nlohmann::json &src);

		void	baseColorEditor(void);
		static constexpr ImVec2					_tabSize = {32.f, 32.f};
		static constexpr ImVec2					_tabPadding = {2.f, 4.f};
		static std::map<std::string, ImVec4>	_baseColors;
		static std::vector<std::string>			_baseColorsLabel;
		std::string								_colorTabSelected{"primary"};

		using ColorList = std::initializer_list<ImGuiCol_>;
		bool	basePopup(Color &color);
		bool	overridePopup(Color &color);
		bool	changeColorPopup(Color &color);
		bool	renderColorRow(const char *name, Color &col);
		void	showColorSection(const char *sectionName,
								const ColorList &colors);
		static constexpr ColorList	_textColors{ImGuiCol_Text,
												ImGuiCol_TextDisabled,
												ImGuiCol_TextLink,
												ImGuiCol_TextSelectedBg};
		static constexpr ColorList	_windowColors{ImGuiCol_WindowBg,
												ImGuiCol_ChildBg,
												ImGuiCol_PopupBg,
												ImGuiCol_MenuBarBg,
												ImGuiCol_Border,
												ImGuiCol_BorderShadow,
												ImGuiCol_FrameBg,
												ImGuiCol_FrameBgHovered,
												ImGuiCol_FrameBgActive,
												ImGuiCol_TitleBg,
												ImGuiCol_TitleBgActive,
												ImGuiCol_TitleBgCollapsed,
												ImGuiCol_ResizeGrip,
												ImGuiCol_ResizeGripHovered,
												ImGuiCol_ResizeGripActive,
												ImGuiCol_ScrollbarBg,
												ImGuiCol_ScrollbarGrab,
												ImGuiCol_ScrollbarGrabHovered,
												ImGuiCol_ScrollbarGrabActive};
		static constexpr ColorList	_widgetColors{ImGuiCol_Button,
												ImGuiCol_ButtonHovered,
												ImGuiCol_ButtonActive,
												ImGuiCol_Header,
												ImGuiCol_HeaderHovered,
												ImGuiCol_HeaderActive,
												ImGuiCol_CheckMark,
												ImGuiCol_SliderGrab,
												ImGuiCol_SliderGrabActive,
												ImGuiCol_InputTextCursor,
												ImGuiCol_Separator,
												ImGuiCol_SeparatorHovered,
												ImGuiCol_SeparatorActive};
		static constexpr ColorList	_tabColors{ImGuiCol_Tab,
												ImGuiCol_TabHovered,
												ImGuiCol_TabSelected,
												ImGuiCol_TabSelectedOverline,
												ImGuiCol_TabDimmed,
												ImGuiCol_TabDimmedSelected,
												ImGuiCol_TabDimmedSelectedOverline};
		static constexpr ColorList	_tableColors{ImGuiCol_TableHeaderBg,
												ImGuiCol_TableBorderStrong,
												ImGuiCol_TableBorderLight,
												ImGuiCol_TableRowBg,
												ImGuiCol_TableRowBgAlt,
												ImGuiCol_PlotLines,
												ImGuiCol_PlotLinesHovered,
												ImGuiCol_PlotHistogram,
												ImGuiCol_PlotHistogramHovered,
												ImGuiCol_TreeLines};
		static constexpr ColorList	_miscColors{ImGuiCol_DragDropTarget,
												ImGuiCol_DragDropTargetBg,
												ImGuiCol_NavCursor,
												ImGuiCol_NavWindowingHighlight,
												ImGuiCol_NavWindowingDimBg,
												ImGuiCol_ModalWindowDimBg,
												ImGuiCol_UnsavedMarker,
												ImGuiCol_DockingPreview,
												ImGuiCol_DockingEmptyBg};

		bool	colorPicker(const std::string &label, ImVec4 &color);
		struct	ColorSelectableStyle {
			const char		*format{"%s"};
			bool			isSelected{false};
			const ImVec2	&tabSize;		
			const ImVec2	&tabPadding;		
		};
		bool	colorSelectable(const std::string &label, const ImVec4 &color,
							const ColorSelectableStyle &style);

		static ImVec4	resolveColor(const Color &col);
		static void		applyPalette(ImGuiCol col, ImVec4 colValue);

		ImVec4	_primaryColor{0.f, 0.f, 0.f, 1.f};
		ImVec4	_secondaryColor{0.f, 0.f, 0.f, 1.f};
		ImVec4	_shadowColor{0.f, 0.f, 0.f, 1.f};
		ImVec4	_highlightColor{0.f, 0.f, 0.f, 1.f};

		static std::map<ImGuiCol_, Color>	_colors;
};

}
