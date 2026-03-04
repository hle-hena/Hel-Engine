/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/04 18:40:31                                        */
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

bool	Engine::beginFrame(VkCommandBuffer commandBuffer,
						Image *colorImage, Image *depthImage) {
	VkCommandBufferBeginInfo	beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		return (true);

	VkClearValue	colorClear = {{{0.1f, 0.1f, 0.1f, 1.0f}}};
	VkClearValue	depthClear = { .depthStencil = {1.0f, 0} };

	auto	extent = colorImage->getExtent();
	auto	colorAttach = colorImage->getRenderingInfo(colorClear,
									VK_ATTACHMENT_LOAD_OP_CLEAR,
									VK_ATTACHMENT_STORE_OP_STORE);
	auto	depthAttach = depthImage->getRenderingInfo(depthClear,
									VK_ATTACHMENT_LOAD_OP_CLEAR,
									VK_ATTACHMENT_STORE_OP_DONT_CARE);

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea = {{0, 0}, colorImage->getExtent()};
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttach;
	renderingInfo.pDepthAttachment = &depthAttach;

	vkCmdBeginRendering(commandBuffer, &renderingInfo);

	VkViewport	viewport{};
	viewport.height = static_cast<float>(extent.height);
	viewport.width = static_cast<float>(extent.width);
	viewport.maxDepth = 1.f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D	scissor{};
	scissor.extent = extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	return (false);
}

bool	Engine::endFrame(VkCommandBuffer commandBuffer) {
	vkCmdEndRendering(commandBuffer);
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		return (true);
	return (false);
}

WindowResources *Engine::getWindowResources(Window& window) {
	if (_perWindowResources.find(&window) != _perWindowResources.end())
		return (&_perWindowResources[&window]);
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

	_perWindowResources[&window] = std::move(newResources);
	return (&_perWindowResources[&window]);
}

void	Engine::renderUI(Window &window, uint32_t currentFrame) {
	WindowResources *resources = getWindowResources(window);
	if (!resources)
		return ;
	VkRenderPass	renderPass = nullptr/* _passes.getRenderPasss(window.getFormat(),
												window.getDepthFormat()) */;
	window.getUI().newFrame(renderPass);
	_uiSystem.render(nullptr, *resources, currentFrame);
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
	if (swapchain.acquireNextImage(window, currentFrame, &imageIndex))
		return ;

	updateGlobalUBO(window, currentFrame);
	WindowResources *resources = getWindowResources(window);
	if (!resources)
		return ;
	Swapchain		&swap = window.getSwapchain();
	VkCommandBuffer	commandBuffer = resources->commandBuffers[currentFrame];
	vkResetCommandBuffer(commandBuffer, 0);

	auto	colorImage = swap.getNextColorImage(imageIndex);
	auto	depthImage = swap.getDepthImage();

	UiContext	&ui = window.getUI();
	beginFrame(commandBuffer, colorImage, depthImage);
	_renderSystem.render(renderPass, *resources, currentFrame);
	_cameraSystem.render(renderPass, *resources, currentFrame);
	// ui.renderFrame(commandBuffer);
	endFrame(commandBuffer);

	swapchain.submitCommandBuffer(&commandBuffer, imageIndex, currentFrame);
	if (swapchain.present(window, imageIndex, currentFrame))
		return ;
}

}
