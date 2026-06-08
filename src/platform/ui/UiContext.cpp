/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UiContext.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 14:42:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/05 17:35:03                                        */
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
#include "core/Application.hpp"
#include "platform/ui/StyleEditor.hpp"

namespace	hel {

UiContext::UiContext(Window *window)
	:	_window{window},
		_device{window->_app.getVkContext().getDevice()} {
}

UiContext::~UiContext(void) {
}

void	UiContext::destroy(void) {
	if (_fullyInitialised) {
		vkDeviceWaitIdle(_device.getLogical());
		ImGui::SetCurrentContext(_context);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(_context);
	}
}

void	UiContext::init() {
	initDescriptorPool(_device);
	initImGui(_device);
}

void	UiContext::initImGui(Device &device) {
	_context = ImGui::CreateContext();
	ImGui::SetCurrentContext(_context);
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplGlfw_InitForVulkan(_window->_windowPtr, true);

	auto	colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkPipelineRenderingCreateInfo	renderingCreateInfo{};
	renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	renderingCreateInfo.colorAttachmentCount = 1;
	renderingCreateInfo.pColorAttachmentFormats = &colorFormat;

	ImGui_ImplVulkan_InitInfo	initInfo{};
	initInfo.ApiVersion = VK_API_VERSION_1_3;
	initInfo.Instance = _window->_instance;
	initInfo.PhysicalDevice = device.getPhysical();
	initInfo.Device = device.getLogical();
	initInfo.QueueFamily = device.getQueueFamily().graphicsFamily.value();
	initInfo.Queue = device.getGraphicsQueue();
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

	sys::StyleEditor::loadFromFile("currentStyle.json");
	sys::StyleEditor::applyPalette();
}

void	UiContext::initDescriptorPool(Device &device) {
	_pool = DescriptorPool::Builder(device)
		.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.f)
		.setPageSize(100)
		.setCreationFlag(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
		.build();
}

VkDescriptorSet	UiContext::registerTexture(Device &device, Image *image,
								VkFormat format, VkImageAspectFlags aspect) {
	VkDescriptorSet	id = ImGui_ImplVulkan_AddTexture(
			Sampler::getSampler(device, {}), image->getView(format, aspect),
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	return (id);
}

void	UiContext::unregisterTexture(VkDescriptorSet texture) {
	ImGui_ImplVulkan_RemoveTexture(texture);
}

void	UiContext::newFrame() {
	if (_fullyInitialised) {
		ImGui::SetCurrentContext(_context);
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		return ;
	}
	init();
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	_fullyInitialised = true;
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
