/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: StyleEditor.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/17 16:32:44 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/20 17:57:07                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "platform/ui/Panel.hpp"

# include "api/ImGui/imgui.h"

# include <map>

namespace	hel::sys {

class	StyleEditor : public Panel<StyleEditor> {
	public:
		enum class	ReferenceColors { Primary, Secondary, Shadow, Highlight };


		struct	Color {
			ReferenceColors	reference{ReferenceColors::Primary};
			ImVec4			color{0.f, 0.f, 0.f, 1.f};
			float			alpha{1.f};
			bool			override{false};
		};

		static constexpr const char	*label = "UI Style";
		StyleEditor(void) = default;
		~StyleEditor(void) = default;

		expected<void, std::string>	onInit(void) override;

		void	render(Window *window, const ImVec2 &) override;

	private:
		bool	colorPicker(const char *label, ImVec4 &color);

		void	addColorTab(int index, const char *label, const ImVec4 &color);
		bool	baseColorEditor(void);
		static constexpr ImVec2			tabSize = {32.f, 32.f};
		static constexpr ImVec2			tabPadding = {2.f, 4.f};
		static std::vector<std::pair<
				const char *, ImVec4>>	_baseColors;
		int								_colorTabSelected{0};

		ImVec4	resolveColor(const Color &col) const;
		void	applyPalette(void);
		void	renderColorRow(const char *name, Color &col);

		ImVec4	_primaryColor{0.f, 0.f, 0.f, 1.f};
		ImVec4	_secondaryColor{0.f, 0.f, 0.f, 1.f};
		ImVec4	_shadowColor{0.f, 0.f, 0.f, 1.f};
		ImVec4	_highlightColor{0.f, 0.f, 0.f, 1.f};

		std::map<ImGuiCol_, Color>	_colors;
};

}
