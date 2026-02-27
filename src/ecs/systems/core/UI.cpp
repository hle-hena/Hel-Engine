/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 17:57:07                                        */
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
}

}
