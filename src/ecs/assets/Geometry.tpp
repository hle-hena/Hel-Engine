/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Geometry.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 19:31:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/23 19:17:34                                        */
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
	auto	stagingBuffer = Buffer::create(device, sizeof(T), data.size(),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer->writeToBuffer(data.data(), size);

	auto	buffer = Buffer::create(device, sizeof(T), data.size(),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


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
