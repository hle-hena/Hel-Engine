/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 20:15:51                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */


#include "ecs/systems/core/UI.hpp"
#include "ecs/Registry.hpp"
#include "api/ImGui/imgui.h"
#include <string>

namespace	hel::sys {

UI::UI(Device &device, Registry &registry)
	:	ISystem(device, registry),
		_assetManager{registry.getAssetManager()} {
}

UI::~UI(void) {
}

void	UI::render(VkRenderPass renderPass, WindowResources &resources,
				uint32_t currentFrame) {
	ImGui::ShowDemoWindow();

	showEntitiesTab();
}

void	UI::moveEntity(View<comp::Hierarchy> view, Entity::id srcHandle,
					Entity::id dstHandle) {
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

void	UI::showEntity(Entity::id handle, View<comp::Hierarchy> view) {
	auto	hierarchy = view.get<comp::Hierarchy>(handle);		
	auto	nameComp = _registry.getComponent<comp::Name>(handle);
	std::string	name = "unknown entity (id: " +
				std::to_string(Entity::getIndex(handle)) + ")";
	if (nameComp)
		name = nameComp->name;
	bool	nodeOpen = ImGui::TreeNode(name.c_str());

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
			moveEntity(view, *static_cast<Entity::id *>(payload->Data), handle);
		}
		ImGui::EndDragDropTarget();
	}

	if (nodeOpen) {
		for (auto chilHandle: hierarchy->childrenId)
			showEntity(chilHandle, view);
		ImGui::TreePop();
	}
}

void	UI::showEntitiesTab(void) {
	static bool	tabOpen = true;

	if (tabOpen) {
		ImGui::Begin("Entities in the scene", &tabOpen);

		auto	view = _registry.view<comp::Hierarchy>();
		for (auto handle: view) {
			auto	hierarchy = view.get<comp::Hierarchy>(handle);
			if (hierarchy->parentId == Entity::NOT_REGISTERED)	
				showEntity(handle, view);
		}

		ImGui::End();
	}
}

}
