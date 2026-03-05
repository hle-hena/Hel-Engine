/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/05 15:22:31 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/05 18:52:26                                        */
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
		ImGui::Text(label);
	}

	ImGui::TableNextColumn();
	ImGui::PushItemWidth(-1.0f);
	if (label)	{ ImGui::PushID(label); }
	else		{ ImGui::PushID("##"); }
	drawAction();
	ImGui::PopID();
	ImGui::PopItemWidth();
}

template <typename T>
void	TableRow::fillVec(std::vector<T> &vec, size_t wantedSize) {
	if (vec.size() != wantedSize)
		vec = std::vector<T>(wantedSize, vec[0]);
}

}
