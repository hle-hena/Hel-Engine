/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InspectorUI.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 21:54:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/28 14:14:24                                        */
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

namespace	hel::sys {

void	InspectorUI::render(Window *window) {
	auto	handle = window->getEntityFocus();
	if (handle == Entity::NOT_REGISTERED)
		return ;
	ImGui::Begin("Inspector");
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
		}
	}
	ImGui::Button("Add a component", {20, 8});
	ImGui::End();
}

void	InspectorUI::setBuiltInDrawFunc(void) {
	setDrawFunc<comp::Transform>([](void *raw){
		auto	transform = static_cast<comp::Transform *>(raw);
		bool	changed;

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
}

}
