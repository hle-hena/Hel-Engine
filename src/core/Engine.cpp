/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/26 17:01:01                                        */
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
		_triangleSystem{device, registry,
					"assets/shaders/triangle.vert.spv",
					"assets/shaders/triangle.frag.spv"},
		_notTriangleSystem{device, registry,
					"assets/shaders/triangle.vert.spv",
					"assets/shaders/triangle.frag.spv"},
		_transformSystem{registry} {
}

Engine::~Engine(void) {
	if (_commandPool != VK_NULL_HANDLE)
		vkDestroyCommandPool(_device.getLogical(), _commandPool, nullptr);
}

bool	Engine::init(void) {
	return (createCommandPool());
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

VkCommandBuffer Engine::getCommandBuffer(Window& window, uint32_t currentFrame) {
	if (_perWindowCommandBuffers.find(&window) == _perWindowCommandBuffers.end()) {
		VkCommandBufferAllocateInfo	allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = _commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = Swapchain::MAX_FRAMES_IN_FLIGHT;

		WindowCmdBuffers newBuffers;
		if (vkAllocateCommandBuffers(_device.getLogical(), &allocInfo, newBuffers.data()) != VK_SUCCESS)
			return (VK_NULL_HANDLE);
		_perWindowCommandBuffers[&window] = newBuffers;
	}
	return _perWindowCommandBuffers[&window][currentFrame];
}

void	Engine::runFrame(Window &window, uint32_t currentFrame) {
	Swapchain	&swapchain = window.getSwapchain();

	uint32_t	imageIndex;
	if (swapchain.acquireNextImage(window, currentFrame, &imageIndex))
		return ;

	VkCommandBuffer	cmd = getCommandBuffer(window, currentFrame);
	if (cmd == VK_NULL_HANDLE)
		return ;
	vkResetCommandBuffer(cmd, 0);

	beginFrame(cmd, imageIndex);
	_triangleSystem.update(cmd, window, imageIndex);
	_notTriangleSystem.update(cmd, window, imageIndex);
	_transformSystem.update();
	endFrame(cmd);

	swapchain.submitCommandBuffer(&cmd, imageIndex, currentFrame);
	if (swapchain.present(window, imageIndex, currentFrame))
		return ;
}

}
