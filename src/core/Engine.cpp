/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 11:18:11                                        */
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
		_renderSystem{device, registry, _setLayout},
		_transformSystem{device, registry, _setLayout},
		_cameraSystem{device, registry, _setLayout},
		_controllerSystem{device, registry, _setLayout} {
	_timer.start();
}

Engine::~Engine(void) {
	if (_commandPool != VK_NULL_HANDLE)
		vkDestroyCommandPool(_device.getLogical(), _commandPool, nullptr);
	if (_setLayout)
		vkDestroyDescriptorSetLayout(_device.getLogical(), _setLayout, nullptr);
	if (_descriptorPool)
		vkDestroyDescriptorPool(_device.getLogical(), _descriptorPool, nullptr);
}

bool	Engine::init(void) {
	return (createCommandPool() || createDescriptorSetLayout() || createDescriptorPool());
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

bool	Engine::createDescriptorSetLayout(void) {
	VkDescriptorSetLayoutBinding	globalUboBinding{};
	globalUboBinding.binding = 0;
	globalUboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	globalUboBinding.descriptorCount = 1;
	globalUboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = 1;
	createInfo.pBindings = &globalUboBinding;

	if (vkCreateDescriptorSetLayout(_device.getLogical(), &createInfo, nullptr, &_setLayout))
		RETURN_SET_UNHEALTHY("Couldn't create the descriptor set layout.", true);
	return (false);
}

bool	Engine::createDescriptorPool(void) {
	VkDescriptorPoolSize	poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = GLFW::_maxInstanceCount *
		static_cast<uint32_t>(Swapchain::MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = 1;
	createInfo.pPoolSizes = &poolSize;
	createInfo.maxSets = poolSize.descriptorCount;

	if (vkCreateDescriptorPool(_device.getLogical(), &createInfo, nullptr, &_descriptorPool))
		RETURN_SET_UNHEALTHY("Couldn't create the descriptor pool.", true);
	return (false);
}

bool	Engine::beginFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo	beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		return (true);
	return (false);
}

bool	Engine::endFrame(VkCommandBuffer commandBuffer) {
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

	std::vector<VkDescriptorSetLayout>	layouts(frameCount, _setLayout);
	VkDescriptorSetAllocateInfo	dsAllocInfo{};
	dsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	dsAllocInfo.descriptorPool = _descriptorPool;
	dsAllocInfo.descriptorSetCount = frameCount;
	dsAllocInfo.pSetLayouts = layouts.data();
	if (vkAllocateDescriptorSets(_device.getLogical(), &dsAllocInfo,
		newResources.globalDescriptorSets.data()))	{ return (nullptr); }

	std::vector<VkWriteDescriptorSet>	writeSets(frameCount);
	std::vector<VkDescriptorBufferInfo>	bufferInfos(frameCount);
	for (size_t i = 0; i < frameCount; i++) {
		newResources.globalUbos[i] = Buffer::create(_device, sizeof(GlobalUBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (!newResources.globalUbos[i])	{ return (nullptr); }
		newResources.globalUbos[i]->map();

		bufferInfos[i].buffer = newResources.globalUbos[i]->getBuffer();
		bufferInfos[i].offset = 0;
		bufferInfos[i].range = sizeof(GlobalUBO);
		writeSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSets[i].dstSet = newResources.globalDescriptorSets[i];
		writeSets[i].dstBinding = 0;
		writeSets[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeSets[i].descriptorCount = 1;
		writeSets[i].pBufferInfo = &bufferInfos[i];
	}
	vkUpdateDescriptorSets(_device.getLogical(), frameCount, writeSets.data(),
							0, nullptr);

	_perWindowResources[&window] = std::move(newResources);
	return (&_perWindowResources[&window]);
}

void	Engine::updateFrame(void) {
	_lastFrameTime = _timer.lapTime();
	_timer.lap();
	_controllerSystem.update(_lastFrameTime);
	_transformSystem.update(_lastFrameTime);
	_cameraSystem.update(_lastFrameTime);
}

void	Engine::updateGlobalUBO(Window &window, uint32_t currentFrame) {
	WindowResources	*resources = getWindowResources(window);
	if (!resources)
		return ;
	GlobalUBO	data{};
	data.viewProjection = glm::mat4{0.f};
	if (auto *camera = _registry.getComponent<Camera>(window.getEntityReference())) {
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
	VkCommandBuffer	commandBuffer = resources->commandBuffers[currentFrame];
	vkResetCommandBuffer(commandBuffer, 0);

	beginFrame(commandBuffer, imageIndex);
	_renderSystem.render(*resources, currentFrame, imageIndex);
	endFrame(commandBuffer);

	swapchain.submitCommandBuffer(&commandBuffer, imageIndex, currentFrame);
	if (swapchain.present(window, imageIndex, currentFrame))
		return ;
}

}
