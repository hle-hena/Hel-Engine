/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/08 16:33:45                                        */
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
	if (!mainRequest)
		return ;

	auto	windowEntityHandle = window->getEntityReference();
	auto	rectMin = ImGui::GetCursorScreenPos() - ImGui::GetStyle().WindowPadding;
	auto	rectMax = rectMin + ImGui::GetContentRegionAvail()
						+ ImGui::GetStyle().WindowPadding * 2;
	auto	col = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
	col.w = (windowEntityHandle == _handle ? 0.75f : 0.f);
	// ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax,
	// 		ImGui::ColorConvertFloat4ToU32(col));
	auto	image = _imagePool->acquire(Image::Config()
			.setWidth(static_cast<uint32_t>(std::max(size.x, 1.f)))
			.setHeight(static_cast<uint32_t>(std::max(size.y, 1.f)))
			.setFormats({VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM})
			.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
			.setAspect(VK_IMAGE_ASPECT_COLOR_BIT));
	auto	depth = _imagePool->acquire(Image::Config()
			.setWidth(static_cast<uint32_t>(std::max(size.x, 1.f)))
			.setHeight(static_cast<uint32_t>(std::max(size.y, 1.f)))
			.setFormats(VK_FORMAT_D32_SFLOAT_S8_UINT)
			.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
			.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
	auto	entityImg = _imagePool->acquire(Image::Config()
			.setWidth(static_cast<uint32_t>(std::max(size.x, 1.f)))
			.setHeight(static_cast<uint32_t>(std::max(size.y, 1.f)))
			.setFormats({VK_FORMAT_R32_UINT})
			.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
			.setUsage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			.setAspect(VK_IMAGE_ASPECT_COLOR_BIT));
	if (!image || !depth || !entityImg)
		return ;
	if (_handle == Entity::NOT_REGISTERED)
		_handle = window->getEntityReference();

	RenderRequest	viewportRequest{
		.requestType = "RenderScene",
		.handle = _handle,
		.origin = ImGui::GetCursorScreenPos(),
		.images = {
			{"mainColor", image},
			{"depth layer", depth},
			{"entity layer", entityImg}
		}
	};

	RenderQueue::push(viewportRequest);
	auto	extent = image->getPhysicalExtent();
	ImVec2	uv1 = {size.x / static_cast<float>(extent.width), size.y / static_cast<float>(extent.height)};

	auto	addImage = [&](const std::string &name, Image *img) {
		
		int i = 0;
		while (true) {
			std::string	imageName = name + std::to_string(i++);
			if (mainRequest->images.contains(imageName))
				continue ;
			mainRequest->images[imageName] = img;
			break ;
		}
	};
	addImage("viewport", image);
	addImage("depth", depth);
	addImage("entity", entityImg);
	mainRequest = nullptr;

	// ImGui::Image(entityImg->getTexture(VK_FORMAT_R32_UINT, VK_IMAGE_ASPECT_COLOR_BIT),
	// ImGui::Image(depth->getTexture(VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT),
	ImGui::Image(image->getTexture(VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT),
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
