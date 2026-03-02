/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/02 13:58:20                                        */
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
#include "core/Engine.hpp"

namespace	hel::sys {

UI::UI(Device &device, Registry &registry)
	:	ISystem(device, registry),
		_inspectorUI{registry},
		_entityHierarchyUI{registry} {
	_inspectorUI.setBuiltInDrawFunc();
}

UI::~UI(void) {
}

void	UI::render(VkRenderPass renderPass, WindowResources &resources,
				uint32_t currentFrame) {
	_entityHierarchyUI.render(resources.window);
	_inspectorUI.render(resources.window);
}

}
