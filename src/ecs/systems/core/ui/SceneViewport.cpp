/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/09 14:37:42                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/SceneViewport.hpp"
#include "api/vulkan/Image.hpp"
#include "api/vulkan/Swapchain.hpp"
#include "api/vulkan/Device.hpp"
#include "platform/window/Window.hpp"
#include "api/ImGui/imgui.h"


namespace	hel::sys {

void	SceneViewport::render(Device &device, Window *window) {
	ImGui::Begin("Viewport");
	Image	*image = window->getSwapchain().getOffImage();
	ImGui::Image(image->getTexture(VK_FORMAT_B8G8R8A8_UNORM),
			{image->getExtent().width, image->getExtent().height});
	ImGui::End();
}

}
