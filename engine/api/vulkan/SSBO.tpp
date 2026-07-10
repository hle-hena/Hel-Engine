/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SSBO.tpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/10 09:24:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/10 12:05:07                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/SSBO.hpp"
#include "api/vulkan/Buffer.hpp"

#include <bit>

namespace	hel {

template <typename T>
expected<void>	SSBO<T>::ensureSize(uint32_t count) {
	if (_count == 0 || count >= std::bit_ceil(_count) || std::bit_ceil(count) < std::bit_ceil(_count)) {
		uint32_t	range = std::bit_ceil(count);
		for (auto i = 0; i < _buffers.size(); i++) {
			_buffers[i] = Buffer::create(*_device, sizeof(T), range,
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
						VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_MAPPED_BIT |
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
			if (!_buffers[i])
				return unexpected("Error when allocating a buffer for an SSBO.");
		}
		_count = count;
	}

	return {};
}

template <typename T>
void	SSBO<T>::init(Device *device) {
	_device = device;
	_count = 0;
}

template <typename T>
expected<void>	SSBO<T>::update(const std::vector<T> &data, uint32_t frameIndex) {
	return ensureSize(data.size())
		.and_then([&]{
			_buffers[frameIndex]->writeToBuffer(data.data(), data.size() * sizeof(T));
		});
}

template <typename T>
VkDescriptorBufferInfo	SSBO<T>::getDescriptorInfo(uint32_t frameIndex) {
	auto	infos = _buffers[frameIndex]->getDescriptorInfo();
	infos.range = sizeof(T) * _count;
	return infos;
}


}
