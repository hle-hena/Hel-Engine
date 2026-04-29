/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EntityHierarchy.cpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/14 19:23:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/29 15:31:57                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/EntityHierarchy.hpp"
#include "ecs/Component.hpp"
#include "platform/ui/UIHelper.hpp"
#include "platform/window/Window.hpp"

#include <algorithm>
#include <ui/ImGui/imgui_stdlib.h>

namespace	hel::sys {

expected<void, std::string>	EntityHierarchy::onInit(void) {
	return {};
}

void	EntityHierarchy::moveEntity(View<include<comp::Hierarchy>> &view,
					Entity::id srcHandle, Entity::id dstHandle) {
	auto	srcHierarchy = view.get<comp::Hierarchy>(srcHandle).modify();

	if (std::find(srcHierarchy->childrenId.begin(),
				srcHierarchy->childrenId.end(), dstHandle) !=
				srcHierarchy->childrenId.end())
		return ;
	if (auto prevHierarchy = _registry->getComponent<comp::Hierarchy>
										(srcHierarchy->parentId).modify()) {
		auto	it = std::find(prevHierarchy->childrenId.begin(),
				prevHierarchy->childrenId.end(),
				srcHandle);
		prevHierarchy->childrenId.erase(it);
	}
	srcHierarchy->parentId = dstHandle;
	if (auto dstHierarchy = _registry->getComponent<comp::Hierarchy>(dstHandle).modify())
		dstHierarchy->childrenId.push_back(srcHandle);
}

void	EntityHierarchy::showEntity(Window *window, View<include<comp::Hierarchy>> view,
					Entity::id handle) {
	if (_registry->getComponent<comp::HideEntityInHierarchyTag>(handle))
		return ;
	auto	hierarchy = view.get<comp::Hierarchy>(handle);
	auto	nameComp = _registry->getComponent<comp::Name>(handle);
	std::string	name = "unknown entity (id: " +
				std::to_string(Entity::getIndex(handle)) + ")";
	if (nameComp)
		name = nameComp->name;
	ImGuiTreeNodeFlags	nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (hierarchy->childrenId.empty()) nodeFlags |= ImGuiTreeNodeFlags_Leaf;

	bool	nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)handle, nodeFlags, "");
	ImGui::SameLine();
	if (ImGui::Selectable(name.c_str(), window->getEntityFocus() == handle))
		window->setEntityFocus(handle);
	if (ImGui::BeginDragDropSource()) {
		Entity::id	payload = handle;
		ImGui::SetDragDropPayload("ENTITY",
								&payload, sizeof(Entity::id));
		ImGui::TextUnformatted(("Entity (" + name + ")").c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropSource()) {
		Entity::id	payload = handle;
		ImGui::SetDragDropPayload("ENTITY",
								&payload, sizeof(Entity::id));
		ImGui::TextUnformatted(("Moving an entity (" + name + ")").c_str());
		ImGui::EndDragDropSource();
	}

	DropTarget("ENTITY")
		.build([&](auto payload){
			moveEntity(view, *static_cast<Entity::id *>(payload->Data), handle);
		});

	if (nodeOpen) {
		for (auto childHandle: hierarchy->childrenId)
			showEntity(window, view, childHandle);
		ImGui::TreePop();
	}
}

void	EntityHierarchy::render(Window *window, const ImVec2 &) {
	if (ImGui::Button("Add a new Entity"))
		_registry->createEntity();
	ImGui::Separator();

	auto	view = _registry->view<include<comp::Hierarchy>>();
	DropTarget("ENTITY")
		.setResetPosition(true)
		.addDummy()
		.build([&](auto payload){
			moveEntity(view, *static_cast<Entity::id *>(payload->Data), Entity::NOT_REGISTERED);
		});

	for (auto handle: view) {
		auto	hierarchy = view.get<comp::Hierarchy>(handle);
		if (hierarchy->parentId == Entity::NOT_REGISTERED)	
			showEntity(window, view, handle);
	}
	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		window->setEntityFocus(Entity::NOT_REGISTERED);
}

}
