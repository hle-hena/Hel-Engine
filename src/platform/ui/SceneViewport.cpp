/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/03 19:06:39                                        */
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
#include "core/Queues.hpp"
#include "platform/ui/UIHelper.hpp"

namespace	hel::sys {

expected<void, std::string>	SceneViewport::onInit(void) {
	return {};
}

void	SceneViewport::render(Window *window, const ImVec2 &size) {
	auto	windowEntityHandle = window->getEntityReference();
	auto	rectMin = ImGui::GetCursorScreenPos() - ImGui::GetStyle().WindowPadding;
	auto	rectMax = rectMin + ImGui::GetContentRegionAvail()
						+ ImGui::GetStyle().WindowPadding * 2;
	auto	col = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
	col.w = (windowEntityHandle == _handle ? 0.75f : 0.f);
	ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax,
			ImGui::ColorConvertFloat4ToU32(col));
	auto	image = _imagePool->acquire(Image::Config()
			.setWidth(static_cast<uint32_t>(std::max(size.x, 1.f)))
			.setHeight(static_cast<uint32_t>(std::max(size.y, 1.f)))
			.setFormats({VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM})
			.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
			.setAspect(VK_IMAGE_ASPECT_COLOR_BIT));
	if (!image)
		return ;
	if (_handle == Entity::NOT_REGISTERED)
		_handle = window->getEntityReference();

	RenderQueue::push({_handle, ImGui::GetCursorScreenPos(), {{"mainColor", image}}});
	auto	extent = image->getPhysicalExtent();
	ImVec2	uv1 = {size.x / static_cast<float>(extent.width), size.y / static_cast<float>(extent.height)};

	ImGui::Image(image->getTexture(VK_FORMAT_B8G8R8A8_UNORM),
			size, {0.f, 0.f}, uv1);
	if (windowEntityHandle == _handle && (
		glfwGetInputMode(window->getWindow(), GLFW_CURSOR) ==
			GLFW_CURSOR_DISABLED || ImGui::IsItemHovered()))
		ImGui::SetNextFrameWantCaptureMouse(false);
	if (ImGui::IsItemClicked())
		window->setEntityReference(_handle);

	DropTarget("ENTITY")
		.setPos(rectMin)
		.setSize(size)
		.addDummy()
		.build([&](auto payload){
			_handle = *static_cast<Entity::id *>(payload->Data);
		});
}

}
