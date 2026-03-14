/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EntityHierarchyUI.cpp                                               */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/28 13:55:54 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 19:36:21                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/EntityHierarchyUI.hpp"
#include "platform/ui/UIHelper.hpp"
#include "api/ImGui/imgui_stdlib.h"
#include "platform/window/Window.hpp"

namespace	hel::sys {

void	EntityHierarchyUI::init(Registry *registry) {
	_registry = registry;
}

void	EntityHierarchyUI::moveEntity(Window *window, View<comp::Hierarchy> &view,
					Entity::id srcHandle, Entity::id dstHandle) {
	auto	srcHierarchy = _registry->modify(view.get
								<comp::Hierarchy>(srcHandle));

	if (std::find(srcHierarchy->childrenId.begin(),
				srcHierarchy->childrenId.end(), dstHandle) !=
				srcHierarchy->childrenId.end())
		return ;
	if (auto prevHierarchy = _registry->modify<comp::Hierarchy>
										(srcHierarchy->parentId)) {
		auto	it = std::find(prevHierarchy->childrenId.begin(),
				prevHierarchy->childrenId.end(),
				srcHandle);
		prevHierarchy->childrenId.erase(it);
	}
	srcHierarchy->parentId = dstHandle;
	if (auto dstHierarchy = _registry->modify<comp::Hierarchy>(dstHandle))
		dstHierarchy->childrenId.push_back(srcHandle);
}

void	EntityHierarchyUI::showEntity(Window *window, View<comp::Hierarchy> view,
					Entity::id handle) {
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

	if (nodeOpen) {
		for (auto childHandle: hierarchy->childrenId)
			showEntity(window, view, childHandle);
		ImGui::TreePop();
	}
}

void	EntityHierarchyUI::render(Window *window, ImVec2 pos, ImVec2 size) {
	ImGuiWindowFlags	windowFlags = ImGuiWindowFlags_NoCollapse |
									ImGuiWindowFlags_NoMove |
									ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(pos);
	ImGui::Begin("Entities in scene", nullptr, windowFlags);

	if (ImGui::Button("Add a new Entity"))
		_registry->createEntity();
	ImGui::Separator();

	auto	view = _registry->view<comp::Hierarchy>();
	ImVec2	cursorPos = ImGui::GetCursorPos();
	ImGui::Dummy(ImGui::GetContentRegionAvail());
	if (ImGui::BeginDragDropTarget()) {
		if (auto payload = ImGui::AcceptDragDropPayload
								("Moving Entity In Hierarchy")) {
			moveEntity(window, view, *static_cast<Entity::id *>(payload->Data), Entity::NOT_REGISTERED);
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::SetCursorPos(cursorPos);

	for (auto handle: view) {
		auto	hierarchy = view.get<comp::Hierarchy>(handle);
		if (hierarchy->parentId == Entity::NOT_REGISTERED)	
			showEntity(window, view, handle);
	}
	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		window->setEntityFocus(Entity::NOT_REGISTERED);

	ImGui::End();
}

}
