/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/14 17:46:38                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Engine.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Swapchain.hpp"
#include "api/vulkan/Sampler.hpp"
#include "api/vulkan/Descriptors.hpp"
#include "api/vulkan/Renderer.hpp"
#include "utils/healthHelper.hpp"
#include "platform/window/Window.hpp"
#include "platform/window/GLFW.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"

#include <stdexcept>
#include <array>
#include <iostream>

namespace hel {

Engine::Engine(Device &device, Registry &registry)
	:	_device{device},
		_registry{registry} {
	_timer.start();
}

Engine::~Engine(void) {
	Sampler::deleteAllSamplers(_device);
	DescriptorFactory::deleteLayoutCache(_device);
	if (_commandPool != VK_NULL_HANDLE)
		vkDestroyCommandPool(_device.getLogical(), _commandPool, nullptr);
}

bool	Engine::init(Window &window) {
	if (createCommandPool())
		return (true);
	createDescriptorPool();
	auto	frameRes = _frame.init(_device, _staticPool.get(), _commandPool);
	if (!frameRes) {
		std::cerr << frameRes.error() << std::endl;
		return (true);
	}
	createImagePool();
	_systems.push_back(std::make_unique<sys::HideMouse>());
	_systems.push_back(std::make_unique<sys::SurfaceAllignement>());
	_systems.push_back(std::make_unique<sys::EditorController>());
	_systems.push_back(std::make_unique<sys::BaseController>());
	_systems.push_back(std::make_unique<sys::Transform>());
	_systems.push_back(std::make_unique<sys::Camera>());
	_systems.push_back(std::make_unique<sys::Render>());
	_systems.push_back(std::make_unique<sys::UI>());

	auto	frameCtx = _frame.getContext(&window, 0, 0);
	_engineCtx.device = &_device;
	_engineCtx.imagePool = _imagePool.get();
	_engineCtx.registry = &_registry;

	for (auto &system: _systems) {
		system->init(_engineCtx, frameCtx);
		system->init();
	}
	return (false);
}

bool	Engine::createCommandPool(void) {
	VkCommandPoolCreateInfo	commandPoolInfo{};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = _device.getQueueFamily().graphicsFamily.value();

	if (vkCreateCommandPool(_device.getLogical(), &commandPoolInfo, nullptr, &_commandPool) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't create the command pool.", true);
	return (false);
}

void	Engine::createDescriptorPool(void) {
	_staticPool = DescriptorPool::Builder(_device)
		.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		.setPageSize(GLFW::_maxInstanceCount * Swapchain::MAX_FRAMES_IN_FLIGHT)
		.build();
}

void	Engine::createImagePool(void) {
	_imagePool = ImagePool::Builder(_device)
		.addImage(Image::Config()
			.setHeight(4096)
			.setWidth(4096)
			.setFormats({VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM})
			.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
			.setUsage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
			.setProperty(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
		.addImage(Image::Config()
			.setHeight(4096)
			.setWidth(4096)
			.setFormats(VK_FORMAT_D32_SFLOAT)
			.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT)
			.setProperty(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
		.build();
}

void	Engine::tick(Window *window, uint32_t frameIndex) {
	auto	frameCtx = _frame.getContext(window, frameIndex, _lastFrameTime);
	auto	&ui = window->getUI();
	_lastFrameTime = _timer.lap();
	_imagePool->releaseAll();

	UITick(ui, frameCtx);
	updateTick(frameCtx);
	renderTick(window, ui, frameCtx, frameIndex);
}

void	Engine::UITick(UiContext &ui, const FrameContext &frameCtx) {
	ui.newFrame();
	for (auto &system: _systems)
		system->registerUI(frameCtx);
	ui.endFrame();
}

void	Engine::updateTick(const FrameContext &frameCtx) {
	for (auto &system: _systems)
		system->update(frameCtx);
}

void	Engine::renderTick(Window *window, UiContext &ui, const FrameContext &frameCtx, uint32_t frameIndex) {
	Swapchain	&swapchain = window->getSwapchain();
	uint32_t	imageIndex;

	if (swapchain.acquireNextImage(*window, frameIndex, &imageIndex))
		return ;
	updateGlobalUBO(*window, frameIndex);
	vkResetCommandBuffer(frameCtx.commandBuffer, 0);

	auto	offImage = _imagePool->get("mainViewport");
	auto	depthImage = _imagePool->acquire(Image::Config()
			.setFormats(VK_FORMAT_D32_SFLOAT)
			.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT)
			.setProperty(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
	auto	swapImage = swapchain.getSwapImage(imageIndex);

	VkCommandBufferBeginInfo	beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(frameCtx.commandBuffer, &beginInfo))
		return ;
	if (auto pass = Renderer(frameCtx.commandBuffer, offImage->getExtent())
					.addColorWrite(offImage, VK_FORMAT_B8G8R8A8_SRGB)
					.addDepthWrite(depthImage, depthImage->getFormat())
					.beginPass()) {
		for (auto &system: _systems)
			system->render(frameCtx, pass._config);
	}

	offImage->transitionLayout(frameCtx.commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	if (auto pass = Renderer(frameCtx.commandBuffer, swapImage->getExtent())
					.addColorWrite(swapImage, VK_FORMAT_B8G8R8A8_UNORM)
					.beginPass()) {
		ui.renderFrame(frameCtx.commandBuffer);
	}

	swapImage->transitionLayout(frameCtx.commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	vkEndCommandBuffer(frameCtx.commandBuffer);

	swapchain.submitCommandBuffer(frameCtx.commandBuffer, imageIndex, frameIndex);
	swapchain.present(*window, imageIndex, frameIndex);
}

void	Engine::updateGlobalUBO(Window &window, uint32_t currentFrame) {
	GlobalUBO	data{};
	data.viewProjection = glm::mat4{0.f};
	if (auto *camera = _registry.getComponent<comp::Camera>(window.getEntityReference())) {
		data.viewProjection = camera->viewProjection;
		data.elapsedTime = _timer.elapsedTime();
	}
	_frame.writeToUBO(&data, currentFrame);
}

}
