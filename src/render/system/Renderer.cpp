/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 16:35:00 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 18:57:43                                        */
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
	return (createCommandPool() || createCommandBuffers());
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

bool	Renderer::createCommandBuffers(void) {
	_commandBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo	allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandBufferCount = Swapchain::MAX_FRAMES_IN_FLIGHT;
	allocateInfo.commandPool = _commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	if (vkAllocateCommandBuffers(_device.getLogical(), &allocateInfo, _commandBuffers.data()) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't allocate the command buffers", true);
	std::cout << "Created the command buffers" << std::endl;
	return (false);
}

void	Renderer::drawFrame(Window &window) {
	VkCommandBuffer	commandBuffer = VK_NULL_HANDLE;
	_meshSystem.render(commandBuffer, window);
}

}
