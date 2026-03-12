/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/12 14:06:23                                        */
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
		_registry{registry},
		_renderSystem{device, registry},
		_transformSystem{device, registry},
		_cameraSystem{device, registry},
		_hideMouseSystem{device, registry},
		_editorControllerSystem{device, registry},
		_baseControllerSystem{device, registry},
		_surfaceAllignementSystem{device, registry},
		_uiSystem{device, registry} {
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
	createImagePool();
	auto	initResources = getWindowResources(window);
	_renderSystem.initAllPipelines(*initResources);
	_transformSystem.initAllPipelines(*initResources);
	_cameraSystem.initAllPipelines(*initResources);
	_hideMouseSystem.initAllPipelines(*initResources);
	_editorControllerSystem.initAllPipelines(*initResources);
	_baseControllerSystem.initAllPipelines(*initResources);
	_surfaceAllignementSystem.initAllPipelines(*initResources);
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

bool	Engine::beginFrame(VkCommandBuffer commandBuffer) {
	VkCommandBufferBeginInfo	beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	return (vkBeginCommandBuffer(commandBuffer, &beginInfo));
}

bool	Engine::endFrame(VkCommandBuffer commandBuffer) {
	return (vkEndCommandBuffer(commandBuffer));
}

WindowResources *Engine::getWindowResources(Window& window) {
	if (_windowResources)
		return (_windowResources.get());
	uint32_t			frameCount = Swapchain::MAX_FRAMES_IN_FLIGHT;
	WindowResources		newResources{&window};

	VkCommandBufferAllocateInfo	cbAllocInfo{};
	cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbAllocInfo.commandPool = _commandPool;
	cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cbAllocInfo.commandBufferCount = frameCount;
	if (vkAllocateCommandBuffers(_device.getLogical(), &cbAllocInfo,
		newResources.commandBuffers.data()))	{ return (nullptr); }

	newResources.descriptorSets = DescriptorFactory(_device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
		.setSetCount(Swapchain::MAX_FRAMES_IN_FLIGHT)
		.build(*_staticPool);

	DescriptorWriter	writer(_device, newResources.descriptorSets.get());
	for (size_t i = 0; i < frameCount; i++) {
		newResources.globalUbos[i] = Buffer::create(_device, sizeof(GlobalUBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (!newResources.globalUbos[i])	{ return (nullptr); }
		newResources.globalUbos[i]->map();

		writer.writeBuffer(i, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, *newResources.globalUbos[i]);
	}
	writer.update();

	_windowResources = std::make_unique<WindowResources>(std::move(newResources));
	return (_windowResources.get());
}

void	Engine::renderUI(Window &window, uint32_t currentFrame) {
	WindowResources *resources = getWindowResources(window);
	if (!resources)
		return ;

	window.getUI().newFrame();
	_uiSystem.render(_imagePool.get(), *resources, currentFrame);
	window.getUI().endFrame();
}

void	Engine::updateFrame(void) {
	_lastFrameTime = _timer.lapTime();
	_timer.lap();
	_hideMouseSystem.update(_lastFrameTime);
	_surfaceAllignementSystem.update(_lastFrameTime);
	_baseControllerSystem.update(_lastFrameTime);
	_editorControllerSystem.update(_lastFrameTime);
	_transformSystem.update(_lastFrameTime);
	_cameraSystem.update(_lastFrameTime);
}

void	Engine::updateGlobalUBO(Window &window, uint32_t currentFrame) {
	WindowResources	*resources = getWindowResources(window);
	if (!resources)
		return ;
	GlobalUBO	data{};
	data.viewProjection = glm::mat4{0.f};
	if (auto *camera = _registry.getComponent<comp::Camera>(window.getEntityReference())) {
		data.viewProjection = camera->viewProjection;
		data.elapsedTime = _timer.elapsedTime();
	}
	resources->globalUbos[currentFrame]->writeToBuffer(&data);
}

void	Engine::renderFrame(Window &window, uint32_t currentFrame) {
	Swapchain	&swapchain = window.getSwapchain();

	uint32_t	imageIndex;
	if (swapchain.acquireNextImage(window, currentFrame, &imageIndex)) {
		_imagePool->release(_imagePool->get("mainViewport"));
		return ;
	}

	updateGlobalUBO(window, currentFrame);
	WindowResources *resources = getWindowResources(window);
	if (!resources)
		return ;
	Swapchain		&swap = window.getSwapchain();
	VkCommandBuffer	commandBuffer = resources->commandBuffers[currentFrame];
	vkResetCommandBuffer(commandBuffer, 0);

	auto	offImage = _imagePool->get("mainViewport");
	auto	depthImage = _imagePool->acquire(Image::Config()
			.setFormats(VK_FORMAT_D32_SFLOAT)
			.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT)
			.setProperty(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

	auto	swapImage = swap.getSwapImage(imageIndex);

	UiContext	&ui = window.getUI();
	beginFrame(commandBuffer);
	if (auto pass = Renderer(commandBuffer, offImage->getExtent())
					.addColorWrite(offImage, VK_FORMAT_B8G8R8A8_SRGB)
					.addDepthWrite(depthImage, depthImage->getFormat())
					.beginPass()) {
		RenderingConfig	config{};
		config.colorFormats.push_back(VK_FORMAT_B8G8R8A8_SRGB);
		config.depthFormat = depthImage->getFormat();

		_renderSystem.render(config, *resources, currentFrame);
		_cameraSystem.render(config, *resources, currentFrame);
	}

	offImage->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	if (auto pass = Renderer(commandBuffer, swapImage->getExtent())
					.addColorWrite(swapImage, VK_FORMAT_B8G8R8A8_UNORM)
					.beginPass()) {
		RenderingConfig	config{};
		config.colorFormats.push_back(VK_FORMAT_B8G8R8A8_UNORM);

		ui.renderFrame(commandBuffer);
	}

	swapImage->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	endFrame(commandBuffer);

	_imagePool->release(offImage);
	_imagePool->release(depthImage);

	swapchain.submitCommandBuffer(&commandBuffer, imageIndex, currentFrame);
	if (swapchain.present(window, imageIndex, currentFrame))
		return ;
}

}
