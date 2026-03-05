/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/05 15:22:31 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/05 18:04:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/UIHelper.hpp"

# include <iostream>

namespace	hel::sys {

template <typename Func>
void	Table::setNextCell(const char *label, Func&& drawAction) {
	ImGui::TableNextColumn();
	if (label != nullptr) {
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", label);
	}

	ImGui::TableNextColumn();
	ImGui::PushItemWidth(-1.0f);
	if (label)	{ ImGui::PushID(label); }
	else		{ ImGui::PushID("##"); }
	drawAction();
	ImGui::PopID();
	ImGui::PopItemWidth();
}

}
