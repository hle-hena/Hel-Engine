/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/29 16:04:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/25 20:24:51                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Device.hpp"

#include <stdexcept>
#include <cstring>
#include <iostream>

namespace	hel {

std::unique_ptr<Buffer>	Buffer::create(Device &device, uint32_t stride,
						uint32_t count, VkBufferUsageFlags usage,
						VmaMemoryUsage memoryUsage,
						VmaAllocationCreateFlags allocFlags) {
	try	 {
		return (std::unique_ptr<Buffer>(new Buffer(device, stride, count, usage, memoryUsage, allocFlags)));
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (nullptr);
	}
}

Buffer::Buffer(Device &device, uint32_t stride,
			uint32_t count, VkBufferUsageFlags usage,
			VmaMemoryUsage memoryUsage,
			VmaAllocationCreateFlags allocFlags)
	:	_device{device} {
	_stride = stride;
	_alignedStride = device.getAligned(stride, usage);
	_size = _alignedStride * count;
	_allocFlags = allocFlags;
	VkBufferCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = _size;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo	allocCreateInfo{};
	allocCreateInfo.usage = memoryUsage;
	allocCreateInfo.flags = allocFlags;
	VmaAllocationInfo		allocInfo;
	if (vmaCreateBuffer(_device.getAllocator(), &createInfo, &allocCreateInfo,
						&_buffer, &_allocation, &allocInfo))
		throw std::runtime_error("Failed to create a buffer");

	if (allocFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
		_mapped = allocInfo.pMappedData;
}

Buffer::~Buffer(void) {
	unmap();
	if (_allocation)
		vmaDestroyBuffer(_device.getAllocator(), _buffer, _allocation);
}

VkResult	Buffer::map(void) {
	if (_mapped)	{ return (VK_SUCCESS); }
	return (vmaMapMemory(_device.getAllocator(), _allocation, &_mapped));
}

void	Buffer::unmap(void) {
	if (_mapped && !(_allocFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT)) {
		vmaUnmapMemory(_device.getAllocator(), _allocation);
		_mapped = nullptr;
	}
}

VkResult	Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
	if (!_mapped)	{ return (VK_SUCCESS); }
	if (size == VK_WHOLE_SIZE)	{ size = _size; }

	return (vmaFlushAllocation(_device.getAllocator(), _allocation, offset, size));
}

void	Buffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
	if (size == VK_WHOLE_SIZE)	{ size = _size; }

	bool	notMapped = (_mapped == nullptr);
	if (notMapped)	{ map(); }
	std::memcpy(static_cast<char *>(_mapped) + offset, data, size);
	if (notMapped)	{ unmap(); }
}

}
