/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/19 13:14:11                                        */
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

#include <iostream>

namespace	hel::sys {

expected<void, std::string>	SceneViewport::onInit(void) {
	return {};
}

void	SceneViewport::render(Window *window, const ImVec2 &size) {
	auto	image = _imagePool->requestRender(Entity::NOT_REGISTERED, Image::Config()
			.setWidth(static_cast<uint32_t>(std::max(size.x, 1.f)))
			.setHeight(static_cast<uint32_t>(std::max(size.y, 1.f)))
			.setFormats({VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM})
			.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
			.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
			.setProperty(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
	if (!image)
		return ;
	auto	extent = image->getPhysicalExtent();
	ImVec2	uv1 = {size.x / extent.width, size.y / extent.height};
	float	aspect = size.x / size.y;
	window->updateEntityReference(aspect);
	ImGui::Image(image->getTexture(VK_FORMAT_B8G8R8A8_UNORM),
			size, {0.f, 0.f}, uv1);
	if (glfwGetInputMode(window->getWindow(), GLFW_CURSOR) ==
			GLFW_CURSOR_DISABLED || ImGui::IsItemHovered())
		ImGui::SetNextFrameWantCaptureMouse(false);
}

}
