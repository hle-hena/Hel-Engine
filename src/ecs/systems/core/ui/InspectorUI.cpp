/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InspectorUI.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 21:54:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/05 16:29:27                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/InspectorUI.hpp"
#include "ecs/systems/core/ui/UIHelper.hpp"
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
	ImGuiWindowFlags	windowFlags = ImGuiWindowFlags_NoCollapse |
									ImGuiWindowFlags_NoTitleBar |
									ImGuiWindowFlags_NoMove |
									ImGuiWindowFlags_NoResize;
	VkExtent2D	extent = window->getExtent();
	ImGui::SetNextWindowSize({_windowWidth, extent.height});
	ImGui::SetNextWindowPos({extent.width - _windowWidth, 0});
	ImGui::Begin("Inspector", nullptr, windowFlags);
	if (ImGui::Button("Remove entity")) {
		removeEntity(handle);
		ImGui::End();
		return ;
	}
	ImGui::SameLine();
	if (handle == window->getEntityReference()) {
		if (ImGui::Button("Unlink window from entity"))
			window->setEntityReference(Entity::NOT_REGISTERED);
	} else {
		if (ImGui::Button("Link window to entity")) {
			window->setEntityReference(handle);
			window->updateEntityReference();
		}
	}
	ImGui::Separator();
	for (auto &[type, pool]: _registry.getPools()) {
		if (pool->has(handle)) {
			auto	label = pool->getTypeName();

			bool	uiOpened = false;
			if (type != typeid(comp::Hierarchy)) {
				bool	isVisible = true;
				uiOpened = ImGui::CollapsingHeader(label, &isVisible,
												ImGuiTreeNodeFlags_DefaultOpen);
				if (!isVisible) {
					pool->removeEntity(handle);
					continue ;
				}
			} else
				uiOpened = ImGui::CollapsingHeader(label,
												ImGuiTreeNodeFlags_DefaultOpen);
			if (uiOpened) {
				auto	it = _drawFuncs.find(type);
				if (it != _drawFuncs.end())
					_drawFuncs[type](window, pool->getRaw(handle));
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

	Splitter(&_windowWidth)
		.setLabel("Inspector splitter")
		.setMin(50.f)
		.setMax(extent.width * 0.25f)
		.setPos({extent.width - _windowWidth, 0.f})
		.setSize(extent.height)
		.setDir(Splitter::Left)
		.build();
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
	setDrawFunc<comp::BaseControllerTag>([](Window *, void *){});
	setDrawFunc<comp::EditorControllerTag>([](Window *, void *){});

	setDrawFunc<comp::Transform>([](Window *window, void *raw){
		auto	transform = static_cast<comp::Transform *>(raw);
		bool	changed = false;

		auto	table = Table("Transform");
		if (table.begin(3)) {
			TableRow(table, window, "Position")
				.setType(TableRow::Type::VecDrag)
				.setSpeed(0.1f)
				.setStart(&transform->position.x)
				.setRange(3)
				.setValueNames({"X:", "Y:", "Z:"})
				.build();
			TableRow(table, window, "Scale")
				.setType(TableRow::Type::VecDrag)
				.setSpeed(0.1f)
				.setStart(&transform->scale.x)
				.setRange(3)
				.setValueNames({"X:", "Y:", "Z:"})
				.build();
			table.end();
		}
		if (changed)
			transform->isDirty = true;
	});

	setDrawFunc<comp::Model>([](Window *, void *raw){
		auto	*model = static_cast<comp::Model *>(raw);

		ImGui::InputText("Model filepath", &model->filePath);
	});

	setDrawFunc<comp::Camera>([](Window *window, void *raw){
		auto	camera = static_cast<comp::Camera *>(raw);
		bool	changed = false;

		changed |= ImGui::DragFloatRange2("Render distance", &camera->near,
									&camera->far, 1.f, 0.001f, 10000.f,
									"Near %.3f", "Far %.3f", ImGuiSliderFlags_AlwaysClamp);
		changed |= DragFloat(window->getWindow(), &camera->fov)
					.setSpeed(0.001f)
					.setMin(1.f)
					.setMax(179.f)
					.setFormat("%.3f°")
					.setLabel("FOV")
					.build();
		ImGui::Text("Aspect ratio :%f", camera->aspect);

		if (changed)
			camera->isDirty = true;
	});

	setDrawFunc<comp::Name>([](Window *, void *raw){
		auto	name = static_cast<comp::Name *>(raw);

		ImGui::InputText("Entity's name", &name->name);
	});

	setDrawFunc<comp::Hierarchy>([](Window *, void *raw){
		auto	hier = static_cast<comp::Hierarchy *>(raw);

		if (hier->parentId != Entity::NOT_REGISTERED)
			ImGui::Text("Parent's id: %d", Entity::getIndex(hier->parentId));
		else
			ImGui::Text("Entity doesn't have a parent");
		for (auto childHandle: hier->childrenId)
			ImGui::BulletText("Child entity %d", Entity::getIndex(childHandle));
	});

	setDrawFunc<comp::SurfaceAllignement>([](Window *, void *raw){
		auto	surface = static_cast<comp::SurfaceAllignement *>(raw);

		if (ImGui::DragFloat3("Up vector", &surface->localUp.x, 0.1f))
			glm::normalize(surface->localUp);
		ImGui::Checkbox("Dynamic allignement", &surface->isDynamic);
	});

	setDrawFunc<comp::Controller>([](Window *window, void *raw){
		auto	controller = static_cast<comp::Controller *>(raw);

		DragFloat(window->getWindow(), &controller->mouseSensivity)
			.setLabel("Mouse sensitivity")
			.build();
		DragFloat(window->getWindow(), &controller->movementSpeed)
			.setLabel("Movement speed")
			.build();
	});
}

}
