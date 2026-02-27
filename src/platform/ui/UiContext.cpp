/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UiContext.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 14:42:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 16:49:07                                        */
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
#include "core/Application.hpp"

namespace	hel {

UiContext::UiContext(Window *window)
	:	_window{window} {
}

UiContext::~UiContext(void) {
}

void	UiContext::destroy(void) {
	if (_fullyInitialised) {
		vkDeviceWaitIdle(_window->_app.getVkContext().getDevice().getLogical());
		ImGui::SetCurrentContext(_context);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(_context);
	}
}

void	UiContext::init(VkRenderPass renderPass) {
	Device	&device = _window->_app.getVkContext().getDevice();
	initDescriptorPool(device);
	initImGui(device, renderPass);
}

void	UiContext::initImGui(Device &device, VkRenderPass renderPass) {
	_context = ImGui::CreateContext();
	ImGui::SetCurrentContext(_context);

	ImGui_ImplGlfw_InitForVulkan(_window->_windowPtr, false);

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
	initInfo.PipelineInfoMain.RenderPass = renderPass;
	ImGui_ImplVulkan_Init(&initInfo);
}

void	UiContext::initDescriptorPool(Device &device) {
	_pool = DescriptorPool::Builder(device)
		.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100.f)
		.setPageSize(Swapchain::MAX_FRAMES_IN_FLIGHT)
		.setCreationFlag(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
		.build();
}

void	UiContext::newFrame(VkRenderPass renderPass) {
	if (_fullyInitialised) {
		ImGui::SetCurrentContext(_context);
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		return ;
	}
	init(renderPass);
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	_fullyInitialised = true;
}

void	UiContext::renderFrame(VkCommandBuffer commandBuffer) {
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

void	UiContext::stealAllCallbacks(GLFWwindow *w) {
	if (_fullyInitialised)
		ImGui_ImplGlfw_InstallCallbacks(w);
}

void	UiContext::dispatchKey(GLFWwindow *w, int key, int scancode,
							int action, int mods) {
	if (_fullyInitialised)
		ImGui_ImplGlfw_KeyCallback(w, key, scancode, action, mods);
}

void	UiContext::dispatchMouse(GLFWwindow *w, int button, int action,
								int mods) {
	if (_fullyInitialised)
		ImGui_ImplGlfw_MouseButtonCallback(w, button, action, mods);
}

void	UiContext::dispatchScroll(GLFWwindow *w, double xoffset,
								double yoffset) {
	if (_fullyInitialised)
		ImGui_ImplGlfw_ScrollCallback(w, xoffset, yoffset);
}

void	UiContext::disptachCursorPos(GLFWwindow *w, double x, double y) {
	if (_fullyInitialised)
		ImGui_ImplGlfw_CursorPosCallback(w, x, y);
}

void	UiContext::dispatchCharCallback(GLFWwindow *w, unsigned int c) {
	if (_fullyInitialised)
		ImGui_ImplGlfw_CharCallback(w, c);
}

void	UiContext::dispatchCursorEnter(GLFWwindow *w, int entered) {
	if (_fullyInitialised)
		ImGui_ImplGlfw_CursorEnterCallback(w, entered);
}

void	UiContext::dispatchFocus(GLFWwindow *w, int focused) {
	if (_fullyInitialised)
		ImGui_ImplGlfw_WindowFocusCallback(w, focused);
}


}