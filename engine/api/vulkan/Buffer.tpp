/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.tpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/11 17:25:36 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/12 20:08:17                                        */
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
expected<std::unique_ptr<Buffer<T>>>	Buffer<T>::create(Device *device,
	const Settings &settings)
{
	auto	buffer = std::unique_ptr<Buffer<T>>(new Buffer());
	if (auto err = buffer->init(device, settings); !err)
		return unexpected(err.error());
	return buffer;
}

template <POD T>
expected<void>	Buffer<T>::init(Device *device, const Settings &settings)
{
	_device = device;
	_settings = settings;
	_stride = sizeof(T);
	if (_settings._dynamicAccess)
		_stride = _device->getAligned(_stride, _settings._usage);
	if (_settings._count.has_value())
		allocate(*_settings._count.value());
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
	create.usage = _settings._usage;
	_availableSize = count * _stride;
	create.size = _availableSize;
	create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo	allocCreate{};
	allocCreate.usage = _settings._memoryUsage;
	allocCreate.flags = _settings._allocFlags;
	VmaAllocationInfo		allocInfo;
	if (vmaCreateBuffer(_device->getAllocator(), &create, &allocCreate,
					&_buffer, &_allocation, &allocInfo) != VK_SUCCESS)
		return unexpected("Couldn't allocate the buffer.");

	if (_settings._allocFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
		_mapped = static_cast<T *>(allocInfo.pMappedData);
}

template <POD T>
expected<void>	Buffer<T>::writeToBuffer(T *data, uint32_t count, uint32_t offset) {
	if (offset + count < _maxCount) {
		if (_settings._count.has_value())
			return unexpected("Trying to write outside of a fixed buffer.");
		deallocate();
		auto	newCount = std::bit_ceil(count + offset);
		if (auto res = allocate(_maxCount); !res)
			return unexpected(res.error());
		_maxCount = newCount;
	}

	std::memcpy(_mapped + offset * _stride, data, count * _stride);
}

}
