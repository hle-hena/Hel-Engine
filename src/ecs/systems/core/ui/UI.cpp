/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 23:06:06                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */


#include "ecs/systems/core/ui/UI.hpp"
#include "ecs/Registry.hpp"
#include "api/ImGui/imgui.h"
#include "api/ImGui/imgui_stdlib.h"
#include <string>

namespace	hel::sys {

UI::UI(Device &device, Registry &registry)
	:	ISystem(device, registry),
		_assetManager{registry.getAssetManager()} {
	_inspectorUI.setDrawFunc<comp::Transform>([](void *raw){
		auto	*t = static_cast<comp::Transform*>(raw);
		bool	changed = false;

		changed |= ImGui::DragFloat3("Position", &t->position.x, 0.1f);
		changed |= ImGui::DragFloat3("Scale", &t->scale.x, 0.1f);

		if (changed) {
			t->isDirty = true;
		}
	});

	_inspectorUI.setDrawFunc<comp::Model>([](void *raw){
		auto	*model = static_cast<comp::Model *>(raw);

		ImGui::InputText("Model filepath", &model->filePath);
	});
}

UI::~UI(void) {
}

void	UI::render(VkRenderPass renderPass, WindowResources &resources,
				uint32_t currentFrame) {
	ImGui::ShowDemoWindow();

	showEntitiesTab();
}

void	UI::moveEntity(View<comp::Hierarchy> view,
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

void	UI::showEntity(View<comp::Hierarchy> view,
					Entity::id handle) {
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

	if (Entity::getIndex(handle) == 1)
		_inspectorUI.renderInspector(_registry, handle);
	if (nodeOpen) {
		for (auto childHandle: hierarchy->childrenId)
			showEntity(view, childHandle);
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
				showEntity(view, handle);
		}

		ImGui::End();
	}
}

}
