/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Dock.cpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:31:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/17 16:11:31                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/Dock.hpp"
#include "platform/ui/UIHelper.hpp"
#include "api/ImGui/imgui.h"

namespace	hel::sys {

void	Dock::render(Window *window) {
	DropTarget("TAB_MOVE")
		.addDummy()
		.build([this](const ImGuiPayload *payload){
			IPanel* panel = *static_cast<IPanel**>(payload->Data);
			panel->changeOwner(this);
		});
	if (ImGui::BeginTabBar("##tabs")) {
		for (auto panel: _panels) {
			bool	open = ImGui::BeginTabItem(panel->getLabel());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				ImGui::SetDragDropPayload("TAB_MOVE", &panel, sizeof(IPanel *));
				ImGui::Text(panel->getLabel());
				ImGui::EndDragDropSource();
			}
			if (open) {
				panel->render(window);
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

}
