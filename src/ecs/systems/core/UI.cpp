/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 15:32:29                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */


#include "ecs/systems/core/UI.hpp"
#include "ecs/Registry.hpp"
#include "core/Engine.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/VulkanInstance.hpp"
#include "api/vulkan/Swapchain.hpp"

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