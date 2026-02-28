/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EntityHierarchyUI.cpp                                               */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/28 13:55:54 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/28 15:50:33                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/EntityHierarchyUI.hpp"
#include "api/ImGui/imgui.h"
#include "api/ImGui/imgui_stdlib.h"
#include "platform/window/Window.hpp"

namespace	hel::sys {

void	EntityHierarchyUI::moveEntity(Window *window, View<comp::Hierarchy> &view,
					Entity::id srcHandle, Entity::id dstHandle) {
	auto	srcHierarchy = _registry.modify(view.get
								<comp::Hierarchy>(srcHandle));
	auto	dstHierarchy = _registry.modify(view.get
								<comp::Hierarchy>(dstHandle));

	if (auto prevHierarchy = _registry.modify<comp::Hierarchy>
										(srcHierarchy->parentId)) {
		auto	it = std::find(prevHierarchy->childrenId.begin(),
				prevHierarchy->childrenId.end(),
				srcHandle);
		prevHierarchy->childrenId.erase(it);
	}
	srcHierarchy->parentId = dstHandle;
	dstHierarchy->childrenId.push_back(srcHandle);
}

void	EntityHierarchyUI::showEntity(Window *window, View<comp::Hierarchy> view,
					Entity::id handle) {
	auto	hierarchy = view.get<comp::Hierarchy>(handle);
	auto	nameComp = _registry.getComponent<comp::Name>(handle);
	std::string	name = "unknown entity (id: " +
				std::to_string(Entity::getIndex(handle)) + ")";
	if (nameComp)
		name = nameComp->name;
	bool	nodeOpen = ImGui::TreeNode(name.c_str());
	bool	nodeClicked = ImGui::IsItemClicked();

	if (ImGui::BeginDragDropSource()) {
		Entity::id	payload = handle;
		ImGui::SetDragDropPayload("Moving Entity In Hierarchy",
								&payload, sizeof(Entity::id));
		ImGui::Text(("Moving an entity (" + name + ")").c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (auto payload = ImGui::AcceptDragDropPayload
								("Moving Entity In Hierarchy")) {
			moveEntity(window, view, *static_cast<Entity::id *>(payload->Data), handle);
		}
		ImGui::EndDragDropTarget();
	}

	if (nodeClicked)
		window->setEntityFocus(handle);
	if (nodeOpen) {
		for (auto childHandle: hierarchy->childrenId)
			showEntity(window, view, childHandle);
		ImGui::TreePop();
	}
}

void	EntityHierarchyUI::render(Window *window) {
	static bool	tabOpen = true;

	if (tabOpen) {
		ImGui::Begin("Entities in the scene", &tabOpen);

		if (ImGui::Button("Add a new Entity"))
			_registry.createEntity();
		ImGui::Separator();

		auto	view = _registry.view<comp::Hierarchy>();
		for (auto handle: view) {
			auto	hierarchy = view.get<comp::Hierarchy>(handle);
			if (hierarchy->parentId == Entity::NOT_REGISTERED)	
				showEntity(window, view, handle);
		}
		// if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
		// 	window->setEntityFocus(Entity::NOT_REGISTERED);

		ImGui::End();
	}
}

}
