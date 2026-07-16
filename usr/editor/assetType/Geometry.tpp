/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Geometry.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 19:31:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/16 11:02:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "assetType/Geometry.hpp"
#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Device.hpp"

#include <iostream>

namespace	hel {

template <typename T>
Ref<Buffer>	Geometry::createBuffer(Device &device,
											std::vector<T> data,
											VkBufferUsageFlags usage) {
	uint32_t	count = static_cast<uint32_t>(data.size());
	auto	newStagingBuffer = Buffer::create<T>(&device, BufferConfig()
								.usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
								.allocFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
								.fixedCount(count));
	auto	newBuffer = Buffer::create<T>(&device, BufferConfig()
		.memoryUsage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
		.usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage)
		.fixedCount(count));

	if (!newStagingBuffer || !newBuffer)
		return {};

	auto	stagingBuffer = *newStagingBuffer;
	auto	buffer = *newBuffer;

	auto	res = stagingBuffer->writeToBuffer(data.data(), count);
	if (!res) {
		std::cerr << res.error() << std::endl;
		return {};
	}

	auto	commandBuffer = device.beginSingleTimeCommand();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = count * stagingBuffer->getStride();
	vkCmdCopyBuffer(commandBuffer, stagingBuffer->getBuffer(),
					buffer->getBuffer(), 1, &copyRegion);

	device.endSingleTimeCommand(commandBuffer);
	return (buffer);
}

}
