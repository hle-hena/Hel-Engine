/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/16 13:05:12                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Engine.hpp"
#include "api/vulkan/Device.hpp"
#include "utils/healthHelper.hpp"
#include "api/vulkan/Swapchain.hpp"
#include "platform/window/Window.hpp"

#include <stdexcept>
#include <array>
#include <iostream>

namespace hel {

Engine::Engine(Device &device, Registry &registry)
	:	_device{device},
		_registry{registry},
		_renderSystem{device, registry, setLayout},
		_transformSystem{device, registry, setLayout},
		_cameraSystem{device, registry, setLayout},
		_controllerSystem{device, registry, setLayout} {
}

Engine::~Engine(void) {
	if (_commandPool != VK_NULL_HANDLE)
		vkDestroyCommandPool(_device.getLogical(), _commandPool, nullptr);
	if (setLayout)
		vkDestroyDescriptorSetLayout(_device.getLogical(), setLayout,
									nullptr);
}

bool	Engine::init(void) {
	return (createCommandPool() || createDescriptorSetLayout());
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
	globalUboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = 1;
	createInfo.pBindings = &globalUboBinding;

	if (vkCreateDescriptorSetLayout(_device.getLogical(), &createInfo, nullptr, &setLayout))
		RETURN_SET_UNHEALTHY("Couldn't create the descriptor set layout.", true);
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

Engine::WindowResources *Engine::getWindowResources(Window& window) {
	if (_perWindowResources.find(&window) == _perWindowResources.end()) {
		VkCommandBufferAllocateInfo	allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = _commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = Swapchain::MAX_FRAMES_IN_FLIGHT;

		WindowResources	newResources;
		if (vkAllocateCommandBuffers(_device.getLogical(), &allocInfo,
									newResources.commandBuffers.data()))
			return (nullptr);
		for (size_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; i++) {
			newResources.globalUbos[i] = Buffer::create(_device,
				sizeof(GlobalUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			newResources.globalUbos[i]->map();
		}
		_perWindowResources[&window] = std::move(newResources);
	}
	return (&_perWindowResources[&window]);
}

void	Engine::updateGlobal(void) {
	_controllerSystem.update();
	_transformSystem.update();
	_cameraSystem.update();
}

void	Engine::runFrame(Window &window, uint32_t currentFrame) {
	Swapchain	&swapchain = window.getSwapchain();

	uint32_t	imageIndex;
	if (swapchain.acquireNextImage(window, currentFrame, &imageIndex))
		return ;

	WindowResources *resources = getWindowResources(window);
	if (!resources)
		return ;
	VkCommandBuffer	commandBuffer = resources->commandBuffers[currentFrame];
	vkResetCommandBuffer(commandBuffer, 0);

	beginFrame(commandBuffer, imageIndex);
	_renderSystem.update(commandBuffer, window, imageIndex);
	endFrame(commandBuffer);

	swapchain.submitCommandBuffer(&commandBuffer, imageIndex, currentFrame);
	if (swapchain.present(window, imageIndex, currentFrame))
		return ;
}

}
