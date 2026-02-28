/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: InspectorUI.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 21:54:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 22:09:31                                        */
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

namespace	hel::sys {

void	InspectorUI::renderInspector(Registry &registry, Entity::id handle) {
	ImGui::Begin("Inspector");
	for (auto &[type, pool]: registry.getPools()) {
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
	ImGui::End();
}

}
