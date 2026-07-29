/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Inspector.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/14 19:23:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/25 17:32:25                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "systems/ui/Inspector.hpp"
#include "systems/Selection.hpp"
#include "systems/EntityReference.hpp"
#include "core/ecs/Registry.hpp"
#include "rhi/window/Window.hpp"
#include "systems/ui/UIHelper.hpp"

#include "core/ecs/ComponentManager.hpp"
#include "core/ecs/Hierarchy.hpp"
#include "components/Camera.hpp"
#include "components/Transform.hpp"
#include "components/Controllers.hpp"
#include "components/Model.hpp"
#include "components/Name.hpp"

#include <ui/ImGui/imgui.h>
#include <ui/ImGui/imgui_stdlib.h>

namespace	hel::sys {

expected<void>	Inspector::onInit(void) {
	setBuiltInDrawFunc();
	return {};
}

void	Inspector::render(const ExecutionContext &ctx, const ImVec2 &) {
	auto	window = ctx.window;
	auto	handle = Selection::getSelected();
	if (handle == Entity::NOT_REGISTERED)
		return ;
	if (ImGui::Button("Remove entity")) {
		removeEntity(window, handle);
		return ;
	}
	ImGui::SameLine();
	if (handle == EntityReference::getReferenced()) {
		if (ImGui::Button("Unlink window from entity"))
			EntityReference::setReferenced(Entity::NOT_REGISTERED);
	} else {
		if (ImGui::Button("Link window to entity")) {
			EntityReference::setReferenced(handle);
		}
	}
	ImGui::Separator();
	for (auto &[type, pool]: _registry->getPools()) {
		if (pool->has(handle)) {
			auto	comp = pool->get(handle);
			auto	typeName_str = std::string(comp.typeName());
			auto	typeName = typeName_str.c_str();

			bool	uiOpened = false;
			if (type != typeid(comp::Hierarchy)) {
				bool	isVisible = true;
				uiOpened = ImGui::CollapsingHeader(typeName, &isVisible,
												ImGuiTreeNodeFlags_DefaultOpen);
				if (!isVisible) {
					pool->removeEntity(handle);
					continue ;
				}
			} else
				uiOpened = ImGui::CollapsingHeader(typeName,
												ImGuiTreeNodeFlags_DefaultOpen);
			if (uiOpened) {
				auto	it = _drawFuncs.find(type);
				if (it != _drawFuncs.end())
					_drawFuncs[type](window, comp);
				else
					ImGui::TextDisabled("No UI integration for %s", typeName);
			}
			ImGui::Separator();
		}
	}
	if (!_addNewComp && ImGui::Button("Add a component"))
		_addNewComp = true;
	addNewComponentPopup(handle);
}

void	Inspector::removeEntity(Window *window, Entity::id handle) {
	auto	hierarchy = _registry->getComponent<comp::Hierarchy>(handle);
	for (auto childHandle: hierarchy->childrenId)
		removeEntity(window, childHandle);
	_registry->removeEntity(handle);
	if (Selection::getSelected() == handle)
		Selection::setSelected(Entity::NOT_REGISTERED);
	if (EntityReference::getReferenced() == handle)
		EntityReference::setReferenced(Entity::NOT_REGISTERED);
}

void	Inspector::addNewComponentPopup(Entity::id handle) {
	if (_addNewComp) {
		auto	&items = ComponentManager::getComponentList();
		ImGui::Combo("Component type", &_newCompTypeIndex,
			[](void *data, int idx) -> const char * {
				const auto	&items = *static_cast<const std::vector<std::string_view> *>(data);

				if (idx < 0 || static_cast<size_t>(idx) >= items.size())
					return "Error";
				return items[static_cast<size_t>(idx)].data();
			},
			const_cast<std::vector<std::string_view> *>(&items),
			static_cast<int>(items.size()));
		if (ImGui::Button("Cancel"))
			_addNewComp = false;
		ImGui::SameLine();
		if (ImGui::Button("Add")) {
			ComponentManager::addComponent(_registry, handle,
				items[static_cast<size_t>(_newCompTypeIndex)]);
			_addNewComp = false;
		}
	}
}

void	Inspector::setBuiltInDrawFunc(void) {
	setDrawFunc<comp::BaseControllerTag>([](Window *, OpaqueComponentHandle &){});
	setDrawFunc<comp::EditorControllerTag>([](Window *, OpaqueComponentHandle &){});

	setDrawFunc<comp::Transform>([](Window *window, OpaqueComponentHandle &comp){
		auto		transform = comp.get<comp::Transform>();
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
		if (!changed)
			comp.dismiss();
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

	setDrawFunc<comp::Model>([](Window *window, OpaqueComponentHandle &comp){
		auto	*model = comp.get<comp::Model>();

		auto	table = Table("Model");
		TableRow(table, window, "Model name")
			.setType(TableRow::Type::InputText)
			.setStart(&model->modelName)
			.build();
	});

	setDrawFunc<comp::Camera>([](Window *window, OpaqueComponentHandle &comp){
		auto	camera = comp.get<comp::Camera>();
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
		if (!changed)
			comp.dismiss();
	});

	setDrawFunc<comp::Name>([](Window *window, OpaqueComponentHandle &comp){
		auto	name = comp.get<comp::Name>();

		auto	table = Table("Name");
		TableRow(table, window, "Entity's name")
			.setType(TableRow::Type::InputText)
			.setStart(&name->name)
			.build();
	});

	setDrawFunc<comp::Hierarchy>([](Window *, OpaqueComponentHandle &comp){
		auto	hier = comp.get<comp::Hierarchy>();

		if (hier->parentId != Entity::NOT_REGISTERED)
			ImGui::Text("Parent's id: %d", Entity::getIndex(hier->parentId));
		else
			ImGui::Text("Entity doesn't have a parent");
		for (auto childHandle: hier->childrenId)
			ImGui::BulletText("Child entity %d", Entity::getIndex(childHandle));
	});

	setDrawFunc<comp::SurfaceAllignement>([](Window *, OpaqueComponentHandle &comp){
		auto	surface = comp.get<comp::SurfaceAllignement>();

		if (ImGui::DragFloat3("Up vector", &surface->localUp.x, 0.1f))
			surface->localUp = glm::normalize(surface->localUp);
		ImGui::Checkbox("Dynamic allignement", &surface->isDynamic);
	});

	setDrawFunc<comp::Controller>([](Window *window, OpaqueComponentHandle &comp){
		auto	controller = comp.get<comp::Controller>();

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
