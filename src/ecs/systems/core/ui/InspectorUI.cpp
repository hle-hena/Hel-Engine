/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InspectorUI.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 21:54:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/28 18:11:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/InspectorUI.hpp"
#include "ecs/Registry.hpp"
#include "api/ImGui/imgui.h"
#include "api/ImGui/imgui_stdlib.h"
#include "platform/window/Window.hpp"
#include "ecs/ComponentList.hpp"

namespace	hel::sys {

void	InspectorUI::render(Window *window) {
	auto	handle = window->getEntityFocus();
	if (handle == Entity::NOT_REGISTERED)
		return ;
	ImGui::Begin("Inspector");
	if (ImGui::Button("Remove entity")) {
		removeEntity(handle);
		ImGui::End();
		return ;
	}
	ImGui::Separator();
	for (auto &[type, pool]: _registry.getPools()) {
		if (pool->has(handle)) {
			auto	label = pool->getTypeName();

			if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen)) {
				auto	it = _drawFuncs.find(type);
				if (it != _drawFuncs.end())
					_drawFuncs[type](pool->getRaw(handle));
				else
					ImGui::TextDisabled("No UI integration for %s", label);
			}
			ImGui::Separator();
		}
	}
	if (!_addNewComp && ImGui::Button("Add a component"))
		_addNewComp = true;
	addNewComponentPopup(handle);
	ImGui::End();
}

void	InspectorUI::removeEntity(Entity::id handle) {
	auto	hierarchy = _registry.getComponent<comp::Hierarchy>(handle);
	for (auto childHandle: hierarchy->childrenId)
		removeEntity(childHandle);
	_registry.removeEntity(handle);
}

void	InspectorUI::addNewComponentPopup(Entity::id handle) {
	if (_addNewComp) {
		auto	items = ComponentList::getComponentList();
		ImGui::Combo("Component type", &_newCompTypeIndex, items.data(), items.size());
		if (ImGui::Button("Cancel"))
			_addNewComp = false;
		ImGui::SameLine();
		if (ImGui::Button("Add")) {
			ComponentList::addComponent(_registry, handle, items[_newCompTypeIndex]);
			_addNewComp = false;
		}
	}
}

void	InspectorUI::setBuiltInDrawFunc(void) {
	setDrawFunc<comp::BaseControllerTag>([](void *){});
	setDrawFunc<comp::EditorControllerTag>([](void *){});

	setDrawFunc<comp::Transform>([](void *raw){
		auto	transform = static_cast<comp::Transform *>(raw);
		bool	changed = false;

		changed |= ImGui::DragFloat3("Position", &transform->position.x, 0.1f);
		changed |= ImGui::DragFloat3("Scale", &transform->scale.x, 0.1f);
		if (changed)
			transform->isDirty = true;
	});

	setDrawFunc<comp::Model>([](void *raw){
		auto	*model = static_cast<comp::Model *>(raw);

		ImGui::InputText("Model filepath", &model->filePath);
	});

	setDrawFunc<comp::Camera>([](void *raw){
		auto	camera = static_cast<comp::Camera *>(raw);
		bool	changed = false;

		changed |= ImGui::DragFloat("near plane", &camera->near, .001f, 0.001f, 1.f);
		changed |= ImGui::DragFloat("far plane", &camera->far,
									.1f, 0.1f, 10000.f);
		changed |= ImGui::DragFloat("FOV", &camera->fov, 1.f, 1.f, 180.f);

		if (changed)
			camera->isDirty = true;
	});

	setDrawFunc<comp::Name>([](void *raw){
		auto	name = static_cast<comp::Name *>(raw);

		ImGui::InputText("Entity's name", &name->name);
	});

	setDrawFunc<comp::Hierarchy>([](void *raw){
		auto	hier = static_cast<comp::Hierarchy *>(raw);

		if (hier->parentId != Entity::NOT_REGISTERED)
			ImGui::Text("Parent's id: %d", Entity::getIndex(hier->parentId));
		else
			ImGui::Text("Entity doesn't have a parent");
		for (auto childHandle: hier->childrenId)
			ImGui::BulletText("Child entity %d", Entity::getIndex(childHandle));
	});

	setDrawFunc<comp::SurfaceAllignement>([](void *raw){
		auto	surface = static_cast<comp::SurfaceAllignement *>(raw);

		if (ImGui::DragFloat3("Up vector", &surface->localUp.x, 0.1f))
			glm::normalize(surface->localUp);
		ImGui::Checkbox("Dynamic allignement", &surface->isDynamic);
	});

	setDrawFunc<comp::Controller>([](void *raw){
		auto	controller = static_cast<comp::Controller *>(raw);

		ImGui::DragFloat("Mouse sensitivity", &controller->mouseSensivity);
		ImGui::DragFloat("Movement speed", &controller->movementSpeed);
	});
}

}
