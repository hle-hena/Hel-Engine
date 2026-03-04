/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EntityHierarchyUI.cpp                                               */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/28 13:55:54 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/04 19:42:54                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/EntityHierarchyUI.hpp"
#include "ecs/systems/core/ui/UIHelper.hpp"
#include "api/ImGui/imgui.h"
#include "api/ImGui/imgui_stdlib.h"
#include "platform/window/Window.hpp"

namespace	hel::sys {

void	EntityHierarchyUI::moveEntity(Window *window, View<comp::Hierarchy> &view,
					Entity::id srcHandle, Entity::id dstHandle) {
	auto	srcHierarchy = _registry.modify(view.get
								<comp::Hierarchy>(srcHandle));

	if (std::find(srcHierarchy->childrenId.begin(),
				srcHierarchy->childrenId.end(), dstHandle) !=
				srcHierarchy->childrenId.end())
		return ;
	if (auto prevHierarchy = _registry.modify<comp::Hierarchy>
										(srcHierarchy->parentId)) {
		auto	it = std::find(prevHierarchy->childrenId.begin(),
				prevHierarchy->childrenId.end(),
				srcHandle);
		prevHierarchy->childrenId.erase(it);
	}
	srcHierarchy->parentId = dstHandle;
	if (auto dstHierarchy = _registry.modify<comp::Hierarchy>(dstHandle))
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

void	EntityHierarchyUI::render(Window *window) {
	ImGuiWindowFlags	windowFlags = ImGuiWindowFlags_NoCollapse |
									ImGuiWindowFlags_NoMove |
									ImGuiWindowFlags_NoResize;
	// auto extent = window->getSwapchain().getExtent();
	VkExtent2D	extent{};
	ImGui::SetNextWindowSize({_windowWidth, extent.height});
	ImGui::SetNextWindowPos({0, 0});
	ImGui::Begin("Entities in scene", nullptr, windowFlags);

	if (ImGui::Button("Add a new Entity"))
		_registry.createEntity();
	ImGui::Separator();

	auto	view = _registry.view<comp::Hierarchy>();
	ImVec2	pos = ImGui::GetCursorPos();
	ImGui::Dummy(ImGui::GetContentRegionAvail());
	if (ImGui::BeginDragDropTarget()) {
		if (auto payload = ImGui::AcceptDragDropPayload
								("Moving Entity In Hierarchy")) {
			moveEntity(window, view, *static_cast<Entity::id *>(payload->Data), Entity::NOT_REGISTERED);
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::SetCursorPos(pos);

	for (auto handle: view) {
		auto	hierarchy = view.get<comp::Hierarchy>(handle);
		if (hierarchy->parentId == Entity::NOT_REGISTERED)	
			showEntity(window, view, handle);
	}
	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		window->setEntityFocus(Entity::NOT_REGISTERED);

	ImGui::End();

	Splitter()
		.setId("Hierarchy splitter")
		.setLimits(50.f, extent.width * 0.5f)
		.setPos(_windowWidth, 0.f)
		.setSize(extent.height)
		.setVal(&_windowWidth)
		.setDir(Splitter::Right)
		.build();
}

}
