/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UiContext.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 14:42:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/29 19:22:49                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/UiContext.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/Descriptors.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Sampler.hpp"
#include "api/vulkan/VulkanContext.hpp"

namespace	hel {

bool								UiContext::_fullyInitialised{false};
std::unique_ptr<DescriptorPool>		UiContext::_pool{nullptr};
std::unordered_map<
			VkDescriptorSet,
			DescriptorSet::ptr>		UiContext::_textures{};
ImGuiContext						*UiContext::_context{nullptr};

void	UiContext::destroy(Device *device) {
	if (_fullyInitialised) {
		vkDeviceWaitIdle(device->getLogical());
		ImGui::SetCurrentContext(_context);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(_context);
		_pool = nullptr;
		_textures.clear();
	}
}

void	UiContext::init(Window *window, Device *device) {
	initDescriptorPool(device);
	initImGui(window, device);
	_fullyInitialised = true;
}

void	UiContext::initImGui(Window *window, Device *device) {
	_context = ImGui::CreateContext();
	ImGui::SetCurrentContext(_context);
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplGlfw_InitForVulkan(window->_windowPtr, true);

	auto	colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkPipelineRenderingCreateInfo	renderingCreateInfo{};
	renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	renderingCreateInfo.colorAttachmentCount = 1;
	renderingCreateInfo.pColorAttachmentFormats = &colorFormat;

	ImGui_ImplVulkan_InitInfo	initInfo{};
	initInfo.ApiVersion = VK_API_VERSION_1_3;
	initInfo.Instance = window->_vkCtx->getInstance();
	initInfo.PhysicalDevice = device->getPhysical();
	initInfo.Device = device->getLogical();
	initInfo.QueueFamily = device->getQueueFamily().graphicsFamily.value();
	initInfo.Queue = device->getGraphicsQueue();
	initInfo.DescriptorPool = _pool->getActivePool();
	initInfo.ImageCount = Swapchain::MAX_FRAMES_IN_FLIGHT;
	initInfo.MinImageCount = Swapchain::MAX_FRAMES_IN_FLIGHT;
	initInfo.UseDynamicRendering = true;
	initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = renderingCreateInfo;
	ImGui_ImplVulkan_Init(&initInfo);

	initImGuiStyle();
}

void	UiContext::initImGuiStyle(void) {
	auto	&style = ImGui::GetStyle();

	style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesToNodes;
	style.TreeLinesRounding = 3.f;
}

void	UiContext::initDescriptorPool(Device *device) {
	_pool = DescriptorPool::Builder(*device)
		.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.f)
		.setPageSize(100)
		.setCreationFlag(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
		.build();
}

VkDescriptorSet	UiContext::registerTexture(Device &device, VkSampler sampler,
										VkImageView view) {
	auto	set = DescriptorFactory(device)
					.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						VK_SHADER_STAGE_FRAGMENT_BIT)
					.setSetCount(1)
					.build(*_pool);

	DescriptorWriter(device, set.get())
		.writeImage(0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, view,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, sampler)
		.update();
	auto	ret = set->sets[0];
	_textures[ret] = std::move(set);
	return (ret);
}

void	UiContext::unregisterTexture(VkDescriptorSet texture) {
	_textures.erase(texture);
}

void	UiContext::newFrame() {
	ImGui::SetCurrentContext(_context);
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void	UiContext::endFrame(void) {
	ImGui::EndFrame();
}

void	UiContext::renderFrame(VkCommandBuffer commandBuffer) {
	ImGui::SetCurrentContext(_context);
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

bool	UiContext::capturesKeyboard(void) {
	if (_fullyInitialised) {
		ImGuiIO	&io = ImGui::GetIO();
		return (io.WantCaptureKeyboard);
	}
	return (false);
}

bool	UiContext::capturesMouse() {
	if (_fullyInitialised) {
		ImGuiIO	&io = ImGui::GetIO();
		return (io.WantCaptureMouse);
	}
	return (false);
}

}
