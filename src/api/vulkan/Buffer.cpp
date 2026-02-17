/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/29 16:04:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/16 12:15:24                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/MemoryHelper.hpp"

#include <stdexcept>
#include <cstring>
#include <iostream>

namespace	hel {

std::unique_ptr<Buffer>	Buffer::create(Device &device, VkDeviceSize size,
					VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	try	 {
		return (std::unique_ptr<Buffer>(new Buffer(device, size, usage, properties)));
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (nullptr);
	}
}

Buffer::Buffer(Device &device, VkDeviceSize size, VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties)
	:	_device{device},
		_size{size} {
	VkBufferCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(_device.getLogical(), &createInfo, nullptr, &_buffer)) {
		throw std::runtime_error("Failed to create a buffer");
	}

	VkMemoryRequirements	memRequirements;
	vkGetBufferMemoryRequirements(_device.getLogical(), _buffer, &memRequirements);
	if (MemoryHelper::allocate(_device, memRequirements, properties, _memory)) {
		throw std::runtime_error("Failed to allocate memory for a buffer");
	}
	vkBindBufferMemory(_device.getLogical(), _buffer, _memory, 0);
}

Buffer::~Buffer(void) {
	unmap();
	if (_buffer != VK_NULL_HANDLE)
		vkDestroyBuffer(_device.getLogical(), _buffer, nullptr);
	if (_memory != VK_NULL_HANDLE)
		vkFreeMemory(_device.getLogical(), _memory, nullptr);
}

VkResult	Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
	if (_mapped)	{ return (VK_SUCCESS); }
	if (size == VK_WHOLE_SIZE)	{ size = _size; }
	return (vkMapMemory(_device.getLogical(), _memory, offset, size, 0, &_mapped));
}

void	Buffer::unmap(void) {
	if (_mapped) {
		vkUnmapMemory(_device.getLogical(), _memory);
		_mapped = nullptr;
	}
}

VkResult	Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
	if (!_mapped)	{ return (VK_SUCCESS); }
	if (size == VK_WHOLE_SIZE)	{ size = _size; }

	VkMappedMemoryRange	mappedMemoryRange{};
	mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedMemoryRange.memory = _memory;
	mappedMemoryRange.offset = offset;
	mappedMemoryRange.size = size;
	return (vkFlushMappedMemoryRanges(_device.getLogical(), 1, &mappedMemoryRange));
}

void	Buffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
	if (size == VK_WHOLE_SIZE)	{ size = _size; }

	bool	notMapped = (_mapped == nullptr);
	if (notMapped)	{ map(); }
	std::memcpy(static_cast<char *>(_mapped) + offset, data, size);
	if (notMapped)	{ unmap(); }
}

}
