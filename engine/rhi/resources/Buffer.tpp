/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.tpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/11 17:25:36 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 10:34:13                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/resources/Buffer.hpp"
#include "rhi/context/Device.hpp"
#include "utils/Logger.hpp"

#include <cstring>

namespace	hel {

template <POD T>
Ref<Buffer>	Buffer::create(Device *device,
	const BufferConfig &config)
{
	Ref<Buffer>	buffer(new Buffer());
	if (auto err = buffer->init<T>(device, config); !err) {
		HEL_FATAL("Failed to create a buffer: {}", err.error());
		return nullptr;
	}
	return buffer;
}

template <POD T>
expected<void>	Buffer::init(Device *device, const BufferConfig &config)
{
	_device = device;
	_config = config;
	_stride = sizeof(T);
	_elementSize = sizeof(T);

	if (_config._dynamicAccess && _config._usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
		_config._descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	else if (_config._dynamicAccess && _config._usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		_config._descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	else if (_config._usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
		_config._descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	else if (_config._usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		_config._descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	if (_config._dynamicAccess)
		_stride = _device->getAligned(_stride, _config._usage);
	if (_config._count != 0)
		return allocate(_config._count)
				.and_then([&](void) -> expected<void>{
					_currentCount = _config._count;
					_range = _currentCount * _stride;
					return {};
				});
	return {};
}

}
