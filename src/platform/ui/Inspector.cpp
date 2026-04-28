/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Inspector.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/14 19:23:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/27 20:14:22                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/Inspector.hpp"
#include "ecs/Registry.hpp"
#include "platform/window/Window.hpp"
#include "ecs/ComponentList.hpp"
#include "ecs/Component.hpp"
#include "platform/ui/UIHelper.hpp"

#include <ui/ImGui/imgui.h>
#include <ui/ImGui/imgui_stdlib.h>

namespace	hel::sys {

expected<void, std::string>	Inspector::onInit(void) {
	setBuiltInDrawFunc();
	return {};
}

void	Inspector::render(Window *window, const ImVec2 &) {
	auto	handle = window->getEntityFocus();
	if (handle == Entity::NOT_REGISTERED)
		return ;
	if (ImGui::Button("Remove entity")) {
		removeEntity(handle);
		return ;
	}
	ImGui::SameLine();
	if (handle == window->getEntityReference()) {
		if (ImGui::Button("Unlink window from entity"))
			window->setEntityReference(Entity::NOT_REGISTERED);
	} else {
		if (ImGui::Button("Link window to entity")) {
			window->setEntityReference(handle);
		}
	}
	ImGui::Separator();
	for (auto &[type, pool]: _registry->getPools()) {
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
}

void	Inspector::removeEntity(Entity::id handle) {
	auto	hierarchy = _registry->getComponent<comp::Hierarchy>(handle);
	for (auto childHandle: hierarchy->childrenId)
		removeEntity(childHandle);
	_registry->removeEntity(handle);
}

void	Inspector::addNewComponentPopup(Entity::id handle) {
	if (_addNewComp) {
		auto	items = ComponentList::getComponentList();
		ImGui::Combo("Component type", &_newCompTypeIndex, items.data(), items.size());
		if (ImGui::Button("Cancel"))
			_addNewComp = false;
		ImGui::SameLine();
		if (ImGui::Button("Add")) {
			ComponentList::addComponent(*_registry, handle, items[_newCompTypeIndex]);
			_addNewComp = false;
		}
	}
}

void	Inspector::setBuiltInDrawFunc(void) {
	setDrawFunc<comp::BaseControllerTag>([](Window *, void *){});
	setDrawFunc<comp::EditorControllerTag>([](Window *, void *){});

	setDrawFunc<comp::Transform>([](Window *window, void *raw){
		auto		transform = static_cast<comp::Transform *>(raw);
		bool		changed = false;
		static bool	displayMat = true;

		auto	table = Table("Transform");
		changed |= TableRow(table, window, "Position")
			.setType(TableRow::Type::VecDrag)
			.setSpeed(0.1f)
			.setStart(&transform->position[0])
			.setRange(3)
			.setValueName({"X:", "Y:", "Z:"})
			.build();
		changed |= TableRow(table, window, "Scale")
			.setType(TableRow::Type::VecDrag)
			.setSpeed(0.1f)
			.setStart(&transform->scale[0])
			.setRange(3)
			.setValueName({"X:", "Y:", "Z:"})
			.build();
		changed |= TableRow(table, window, "Rotation")
			.setType(TableRow::Type::VecDrag)
			.setSpeed(0.001f)
			.setMin(-1.f)
			.setMax(1.f)
			.setRange(4)
			.setValueName({"X:", "Y:", "Z:", "W:"})
			.setStart(&transform->rotation[0])
			.build();
		if (changed)
			transform->isDirty = true;
		table.newRow({Table::WStretch});
		table.setNextCell([&]{
			if (ImGui::Button(displayMat ? "Click to hide the resulting matrix"
									: "Click to display the resulting matrix", {-1.f, 0.f}))
				displayMat = !displayMat;
		});
		if (displayMat) {
			Table::ColumnSizing	sizing(4, Table::WStretch);
			for (auto i = 0; i < 4; i++) {
				table.newRow(sizing);
				for (auto j = 0; j < 4; j++) {
					table.setNextCell([&]{
						ImGui::PushID(i * 4 + j);
						ImGui::Text("%.3f", transform->worldMatrix[j][i]);
						ImGui::PopID();
					});
				}
			}
		}
	});

	setDrawFunc<comp::Model>([](Window *window, void *raw){
		auto	*model = static_cast<comp::Model *>(raw);

		auto	table = Table("Model");
		TableRow(table, window, "Model name")
			.setType(TableRow::Type::InputText)
			.setStart(&model->modelName)
			.build();
	});

	setDrawFunc<comp::Camera>([](Window *window, void *raw){
		auto	camera = static_cast<comp::Camera *>(raw);
		bool	changed = false;

		auto	table = Table("Camera");
		changed |= TableRow(table, window, "Render distance")
			.setType(TableRow::Type::DragRange)
			.setStart(&camera->near)
			.setMin(0.001f)
			.setMax(10000.f)
			.setSpeed({0.001f, 1.f})
			.setFormat({"%.3f", "%.0f"})
			.setValueName({"Near:", "Far:"})
			.build();
		changed |= TableRow(table, window, "FOV")
			.setType(TableRow::Type::VecDrag)
			.setStart(&camera->fov)
			.setMin(1.f)
			.setMax(179.f)
			.setSpeed(0.1f)
			.setFormat("%.1f°")
			.build();

		if (changed)
			camera->isDirty = true;
	});

	setDrawFunc<comp::Name>([](Window *window, void *raw){
		auto	name = static_cast<comp::Name *>(raw);

		auto	table = Table("Name");
		TableRow(table, window, "Entity's name")
			.setType(TableRow::Type::InputText)
			.setStart(&name->name)
			.build();
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

		auto	table = Table("Controller");
		TableRow(table, window, "Mouse sensitivity")
			.setType(TableRow::Type::VecDrag)
			.setStart(&controller->mouseSensivity)
			.setMin(0.0001f)
			.setMax(1.f)
			.setSpeed(0.0001f)
			.setFormat("%.4f")
			.build();
		TableRow(table, window, "Movement speed")
			.setType(TableRow::Type::VecDrag)
			.setStart(&controller->movementSpeed)
			.setMin(0.f)
			.setSpeed(0.1f)
			.setFormat("%.1f")
			.build();
	});
}

}
