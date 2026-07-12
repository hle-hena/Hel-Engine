/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/11 17:20:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/12 20:18:07                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <type_traits>
#include <memory>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <optional>

#include "HelExpected.hpp"
#include "utils/Setters.hpp"

namespace	hel {

class	Device;

template <typename T>
concept	POD = std::is_trivial_v<T>;

template <POD T>
class	Buffer {
	public:
		struct	Settings {
			public:
				SETTER_VERBOSE(usage, VkBufferUsageFlags);
				SETTER_VERBOSE(allocFlags, VkBufferUsageFlags);
				SETTER_VERBOSE(memoryUsage, VmaMemoryUsage);
				SETTER_VERBOSE(count, uint32_t);
				SETTER_VERBOSE(dynamicAccess, bool);

			private:
				VkBufferUsageFlags			_usage;
				VmaAllocationCreateFlags	_allocFlags;
				VmaMemoryUsage				_memoryUsage;
				std::optional<uint32_t>		_count;
				bool						_dynamicAccess{false};
		};

		static expected<std::unique_ptr<Buffer>>	create(Device *device,
						const Settings &settings);

		expected<void>	writeToBuffer(T *data, uint32_t count = 1,
									uint32_t offset = 0);


		VkDescriptorBufferInfo	getDescriptorInfo(void) const//Might need to set the range to something other than _stride ?
			{ return {_buffer, 0, _stride}; }
		VkBuffer				getBuffer(void) const
			{ return (_buffer); }
		void					*getMapped(void) const
			{ return (_mapped); }
		VkDeviceSize			getSize(void) const
			{ return (_availableSize); }
		uint32_t				getStride(void) const
			{ return (_stride); }

	private:
		Buffer(void) = default;
		~Buffer(void) = default;
		Buffer(const Buffer &) = delete;
		Buffer	&operator=(const Buffer &) = delete;
		Buffer(const Buffer &&) = delete;
		Buffer	&operator=(const Buffer &&) = delete;

		expected<void>	init(Device *device, const Settings &settings);

		void			deallocate(void);
		expected<void>	allocate(uint32_t count);

		Device			*_device;

		Settings		_settings;
		uint32_t		_stride;
		uint32_t		_maxCount{0u};
		VkDeviceSize	_availableSize{0};
		VkDeviceSize	_range{0};

		VmaAllocation	_allocation{VK_NULL_HANDLE};
		VkBuffer		_buffer{VK_NULL_HANDLE};
		T				*_mapped{nullptr};
};

}

#include "api/vulkan/Buffer.tpp"
