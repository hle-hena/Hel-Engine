/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 16:35:00 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 22:33:25                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/system/Renderer.hpp"
#include "render/vulkan/Device.hpp"
#include "utils/healthHelper.hpp"
#include "render/vulkan/Swapchain.hpp"
#include "platform/window/Window.hpp"

#include <stdexcept>
#include <array>
#include <iostream>

namespace hel {

Renderer::Renderer(Device &device)
	:	_device{device},
		_meshSystem{device,
					"assets/shaders/triangle.vert.spv",
					"assets/shaders/triangle.frag.spv"} {
}

Renderer::~Renderer(void) {
	if (_commandPool != VK_NULL_HANDLE)
		vkDestroyCommandPool(_device.getLogical(), _commandPool, nullptr);
}

bool	Renderer::init(void) {
	return (createCommandPool());
}

bool	Renderer::createCommandPool(void) {
	VkCommandPoolCreateInfo	commandPoolInfo{};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = _device.getQueueFamily().graphicsFamily.value();

	if (vkCreateCommandPool(_device.getLogical(), &commandPoolInfo, nullptr, &_commandPool) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't create the command pool.", true);
	return (false);
}

bool	Renderer::beginFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo	beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		return (true);
	return (false);
}

bool	Renderer::endFrame(VkCommandBuffer commandBuffer) {
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		return (true);
	return (false);
}

VkCommandBuffer Renderer::getCommandBuffer(Window& window, uint32_t currentFrame) {
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

void	Renderer::drawFrame(Window &window, uint32_t currentFrame) {
	Swapchain	&swapchain = window.getSwapchain();

	uint32_t	imageIndex = swapchain.acquireNextImage(currentFrame);

	VkCommandBuffer	cmd = getCommandBuffer(window, currentFrame);
	if (cmd == VK_NULL_HANDLE)
		return ;
	vkResetCommandBuffer(cmd, 0);

	beginFrame(cmd, imageIndex);
	_meshSystem.render(cmd, window, imageIndex);
	endFrame(cmd);

	swapchain.submitCommandBuffer(&cmd, imageIndex, currentFrame);
	swapchain.present(imageIndex, currentFrame);
}

}
