/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/05 15:22:31 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/05 16:28:50                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/UIHelper.hpp"

namespace	hel::sys {

template <typename Func>
void	Table::setNextCell(const char *label, Func&& drawAction) {
	ImGui::TableNextColumn();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("%s", label);

	ImGui::TableNextColumn();
	ImGui::PushItemWidth(-1.0f);
	ImGui::PushID(label);
	drawAction();
	ImGui::PopID();
	ImGui::PopItemWidth();
}

}
