/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.tpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/11 17:25:36 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/13 12:24:22                                        */
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
#include <cstring>

namespace	hel {

template <POD T>
expected<Ref<Buffer<T>>>	Buffer<T>::create(Device *device,
	const BufferConfig &config)
{
	Ref<Buffer>	buffer(new Buffer());
	if (auto err = buffer->init(device, config); !err)
		return unexpected(err.error());
	return buffer;
}

template <POD T>
Buffer<T>::~Buffer(void) {
	deallocate();
}

template <POD T>
expected<void>	Buffer<T>::init(Device *device, const BufferConfig &config)
{
	_device = device;
	_config = config;
	_stride = sizeof(T);
	if (_config._dynamicAccess)
		_stride = _device->getAligned(_stride, _config._usage);
	if (_config._count.has_value())
		return allocate(_config._count.value());
	return {};
}

template <POD T>
void	Buffer<T>::deallocate(void) {
	_maxCount = 0;
	if (_allocation != VK_NULL_HANDLE)
		vmaDestroyBuffer(_device->getAllocator(), _buffer, _allocation);
}

template <POD T>
expected<void>	Buffer<T>::allocate(uint32_t count) {
	VkBufferCreateInfo		create;
	create.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	create.usage = _config._usage;
	_availableSize = count * _stride;
	create.size = _availableSize;
	create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo	allocCreate{};
	allocCreate.usage = _config._memoryUsage;
	allocCreate.flags = _config._allocFlags;
	VmaAllocationInfo		allocInfo;
	if (vmaCreateBuffer(_device->getAllocator(), &create, &allocCreate,
					&_buffer, &_allocation, &allocInfo) != VK_SUCCESS)
		return unexpected("Couldn't allocate the buffer.");

	if (_config._allocFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
		_mapped = static_cast<T *>(allocInfo.pMappedData);
	return {};
}

template <POD T>
expected<Ref<Buffer<T>>>	Buffer<T>::writeToBuffer(T *data, uint32_t count, uint32_t offset)
{
	Ref<Buffer<T>>	oldBuffer;

	if (offset + count > _maxCount) {
		if (_config._count.has_value())
			return unexpected("Trying to write outside of a fixed buffer.");

		Ref<Buffer<T>>	old(new Buffer());
		old->_device		= _device;
		old->_config		= _config;
		old->_stride		= _stride;
		old->_maxCount		= _maxCount;
		old->_availableSize	= _availableSize;
		old->_allocation	= _allocation;
		old->_buffer		= _buffer;
		old->_mapped		= _mapped;
		oldBuffer = old;

		_allocation = VK_NULL_HANDLE;
		_buffer     = VK_NULL_HANDLE;
		_mapped     = nullptr;
		_maxCount   = 0;
		auto	newCount = std::bit_ceil(count + offset);
		if (auto res = allocate(newCount); !res)
			return unexpected(res.error());
		_maxCount = newCount;
	}

	std::memcpy(_mapped + offset * _stride, data, count * _stride);
	return oldBuffer;
}

}
