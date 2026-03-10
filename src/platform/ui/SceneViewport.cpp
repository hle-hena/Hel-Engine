/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/10 16:56:54                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/SceneViewport.hpp"
#include "api/vulkan/Image.hpp"
#include "api/vulkan/Swapchain.hpp"
#include "platform/window/Window.hpp"


namespace	hel::sys {

void	SceneViewport::render(Window *window, ImVec2 pos, ImVec2 size) {
	ImGuiWindowFlags	windowFlags = ImGuiWindowFlags_NoCollapse |
								ImGuiWindowFlags_NoTitleBar |
								ImGuiWindowFlags_NoMove |
								ImGuiWindowFlags_NoResize |
								ImGuiWindowFlags_NoScrollbar |
								ImGuiWindowFlags_NoMouseInputs |
								ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(pos);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.f, 0.f});
	ImGui::Begin("Viewport", nullptr, windowFlags);
	Image	*image = window->getSwapchain().getOffImage();
	ImGui::Image(image->getTexture(VK_FORMAT_B8G8R8A8_UNORM),
			{image->getExtent().width, image->getExtent().height});
	ImGui::End();
	ImGui::PopStyleVar();
}

}
