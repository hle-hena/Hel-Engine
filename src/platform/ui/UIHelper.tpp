/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/05 15:22:31 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/17 16:25:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/UIHelper.hpp"

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

template <typename Func>
void	DropTarget::build(Func &&dropAction) {
	if (ImGui::BeginDragDropTarget()) {
		ImGuiDragDropFlags	flags = ImGuiDragDropFlags_AcceptNoDrawDefaultRect |
									ImGuiDragDropFlags_AcceptBeforeDelivery;
		if (auto payload = ImGui::AcceptDragDropPayload(_type, flags)) {
			if (payload->Preview) {
				ImVec2	rectMin = ImGui::GetItemRectMin();
				ImVec2	rectMax = ImGui::GetItemRectMax();
				ImGui::GetForegroundDrawList()->AddRectFilled(rectMin, rectMax, IM_COL32(255, 0, 0, 50));
			}
			if (payload->IsDelivery())
				dropAction(payload);
		}
		ImGui::EndDragDropTarget();
	}
}

template <typename Func>
Dummy::Dummy(Func &&dummyAction) {
	ImVec2	cursorPos = ImGui::GetCursorPos();
	ImGui::Dummy(ImGui::GetContentRegionAvail());
	dummyAction();
	ImGui::SetCursorPos(cursorPos);
}

}
