/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/13 16:21:31 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/15 17:36:03                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Device.hpp"

#include <bit>

namespace	hel {

Buffer::~Buffer(void) {
	deallocate();
}

VkDescriptorBufferInfo	Buffer::getDescriptorInfo(uint32_t offset) {
	return _config._dynamicAccess
				? VkDescriptorBufferInfo{_buffer, offset * _stride, _stride}
				: VkDescriptorBufferInfo{_buffer, offset * _stride, _range};
}

void	Buffer::deallocate(void) {
	if (_allocation != VK_NULL_HANDLE)
		vmaDestroyBuffer(_device->getAllocator(), _buffer, _allocation);
	_maxCount		= 0;
	_currentCount	= 0;
	_allocation		= VK_NULL_HANDLE;
	_buffer			= VK_NULL_HANDLE;
	_mapped			= nullptr;
}

expected<void>	Buffer::allocate(uint32_t count) {
	count = std::bit_ceil(count);
	VkBufferCreateInfo		create{};
	create.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	create.usage = _config._usage;
	create.size = count * _stride;
	create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo	allocCreate{};
	allocCreate.usage = _config._memoryUsage;
	allocCreate.flags = _config._allocFlags;
	VmaAllocationInfo		allocInfo;
	if (vmaCreateBuffer(_device->getAllocator(), &create, &allocCreate,
					&_buffer, &_allocation, &allocInfo) != VK_SUCCESS)
		return unexpected("Couldn't allocate the buffer.");

	_maxCount = count;
	_availableSize = create.size;
	if (_config._allocFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
		_mapped = allocInfo.pMappedData;
	return {};
}

expected<Ref<Buffer>>	Buffer::writeToBuffer(void *data, uint32_t count, uint32_t offset)
{
	Ref<Buffer>	oldBuffer;

	if (offset + count > _maxCount) {
		if (_config._fixedCount)
			return unexpected("Trying to write outside of a fixed buffer.");

		Ref<Buffer>	old(new Buffer());
		old->_device		= _device;
		old->_config		= _config;
		old->_stride		= _stride;
		old->_maxCount		= _maxCount;
		old->_availableSize	= _availableSize;
		old->_allocation	= _allocation;
		old->_buffer		= _buffer;
		old->_mapped		= _mapped;
		oldBuffer = old;

		_maxCount		= 0;
		_currentCount	= 0;
		_allocation		= VK_NULL_HANDLE;
		_buffer			= VK_NULL_HANDLE;
		_mapped			= nullptr;
		if (auto res = allocate(count + offset); !res)
			return unexpected(res.error());
		_currentCount = count + offset;
		_range = _currentCount * _stride;
	}

	if (_mapped)
		std::memcpy(static_cast<char *>(_mapped) + offset * _stride,
					data, count * _stride);
	return oldBuffer;
}

}
