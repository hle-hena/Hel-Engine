/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/29 16:04:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/30 11:45:06                                        */
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

namespace	hel {

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
		throw std::runtime_error("failed to create vertex buffer!");//TODO -> find a way to handle this cleanly
	}

	VkMemoryRequirements	memRequirements;
	vkGetBufferMemoryRequirements(_device.getLogical(), _buffer, &memRequirements);
	VkMemoryAllocateInfo	allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
	allocateInfo.allocationSize = memRequirements.size;
	if (vkAllocateMemory(_device.getLogical(), &allocateInfo, nullptr, &_memory)) {
		throw std::runtime_error("Too bad");
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

uint32_t	Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties	deviceProperties;
	vkGetPhysicalDeviceMemoryProperties(_device.getPhysical(), &deviceProperties);

	for (uint32_t i = 0; i < deviceProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (deviceProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return (i);
	}
	throw std::runtime_error("Hum, nope");
}

VkResult	Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
	if (_mapped)	return (VK_SUCCESS);
	if (size == VK_WHOLE_SIZE)
		size = _size;
	return (vkMapMemory(_device.getLogical(), _memory, offset, size, 0, &_mapped));
}

void	Buffer::unmap(void) {
	if (_mapped) {
		vkUnmapMemory(_device.getLogical(), _memory);
		_mapped = nullptr;
	}
}

void	Buffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
	if (size == VK_WHOLE_SIZE)
		size = _size;

	bool	notMapped = (_mapped == nullptr);
	if (notMapped)	map();
	std::memcpy(_mapped + offset, data, _size);
	if (notMapped)	unmap();
}

}
