/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Geometry.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 19:31:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/25 20:23:25                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/assets/Geometry.hpp"
#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Device.hpp"

namespace	hel {

template <typename T>
std::unique_ptr<Buffer>	Geometry::createBuffer(Device &device,
											std::vector<T> data,
											VkBufferUsageFlags usage) {
	VkDeviceSize	size = sizeof(T) * data.size();
	auto	stagingBuffer = Buffer::create(device, sizeof(T),
		static_cast<uint32_t>(data.size()),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

	stagingBuffer->writeToBuffer(data.data(), size);

	auto	buffer = Buffer::create(device, sizeof(T),
		static_cast<uint32_t>(data.size()),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);


	auto	commandBuffer = device.beginSingleTimeCommand();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, stagingBuffer->getBuffer(),
					buffer->getBuffer(), 1, &copyRegion);

	device.endSingleTimeCommand(commandBuffer);
	return (buffer);
}

}
