/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/11 17:06:06                                        */
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
#include "api/vulkan/ImagePool.hpp"


namespace	hel::sys {

void	SceneViewport::render(ImagePool *imagePool, Window *window, ImVec2 pos, ImVec2 size) {
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
	auto	image = imagePool->acquire(Image::Config()
			.setHeight(4096)
			.setWidth(4096)
			.setFormats({VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM})
			.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
			.setUsage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
			.setProperty(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
	// Image	*image = window->getSwapchain().getOffImage();
	ImGui::Image(image->getTexture(VK_FORMAT_B8G8R8A8_UNORM),
			{image->getExtent().width, image->getExtent().height});
	ImGui::End();
	imagePool->release(image);
	ImGui::PopStyleVar();
}

}
