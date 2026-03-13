/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Frame.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 15:47:35 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 18:22:42                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Frame.hpp"
#include "api/vulkan/Device.hpp"

namespace	hel {

tl::expected<void, std::string>	Frame::init(Device &device,
										DescriptorPool &descriptorPool,
										VkCommandPool commandPool) {
	uint32_t			frameCount = Swapchain::MAX_FRAMES_IN_FLIGHT;

	VkCommandBufferAllocateInfo	cbAllocInfo{};
	cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbAllocInfo.commandPool = commandPool;
	cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cbAllocInfo.commandBufferCount = frameCount;
	if (vkAllocateCommandBuffers(device.getLogical(), &cbAllocInfo,
		_commandBuffers.data()))
		return (unexpected("Failed to allocated command buffers"));

	_descriptorSets = DescriptorFactory(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
		.setSetCount(Swapchain::MAX_FRAMES_IN_FLIGHT)
		.build(descriptorPool);

	DescriptorWriter	writer(device, _descriptorSets.get());
	for (size_t i = 0; i < frameCount; i++) {
		_globalUbos[i] = Buffer::create(device, sizeof(GlobalUBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (!_globalUbos[i])	return (unexpected("Failed to create a UBO"));
		_globalUbos[i]->map();

		writer.writeBuffer(i, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, *_globalUbos[i]);
	}
	writer.update();
	return {};
}

FrameContext	Frame::getContext(Window *window, uint32_t frameIndex) {
	FrameContext	ctx{};
	ctx.window = window;
	ctx.commandBuffer = _commandBuffers[frameIndex];
	ctx.globalSet = _descriptorSets->sets[frameIndex];
	ctx.globalLayout = _descriptorSets->setLayout;
}

}
