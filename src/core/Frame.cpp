/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Frame.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 15:47:35 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/30 16:49:16                                        */
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
										DescriptorPool *descriptorPool,
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
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
		.setSetCount(Swapchain::MAX_FRAMES_IN_FLIGHT)
		.build(*descriptorPool);

	DescriptorWriter	writer(device, _descriptorSets.get());
	for (uint32_t i = 0; i < frameCount; i++) {
		_globalUbos[i] = Buffer::create(device, sizeof(GlobalUBO), 32,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
		if (!_globalUbos[i])	return (unexpected("Failed to create a UBO"));
		_globalUbos[i]->map();

		writer.writeBuffer(i, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, *_globalUbos[i]);

		_dynamicPools[i] = DescriptorPool::Builder(device)
			.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1.f)
			.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f)
			.addDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10.f)
			.setPageSize(1000)
			.build();
	}
	writer.update();
	return {};
}

FrameContext	Frame::getContext(Window *window, uint32_t frameIndex,
								float deltaTime) {
	return {
		.window = window,
		.commandBuffer = _commandBuffers[frameIndex],
		.globalSet = _descriptorSets->sets[frameIndex],
		.globalLayout = _descriptorSets->setLayout,
		.descriptorPool = _dynamicPools[frameIndex].get(),
		.deltaTime = deltaTime,
		.frameIndex = frameIndex
	};
}

void	Frame::writeToUBO(GlobalUBO *data, uint32_t passIndex, uint32_t frameIndex) {
	uint32_t	offset = _globalUbos[0]->alignedStride() * passIndex;
	_globalUbos[frameIndex]->writeToBuffer(data, sizeof(GlobalUBO), offset);
}

}
