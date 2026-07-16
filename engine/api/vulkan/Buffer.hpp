/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/11 17:20:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/16 14:58:03                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <type_traits>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "HelExpected.hpp"
#include "utils/Setters.hpp"
#include "utils/Ref.hpp"

namespace	hel {

class	Device;

template <typename T>
concept	POD = std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>;

struct	BufferConfig {
	public:
		SETTER_VERBOSE(usage, VkBufferUsageFlags);
		SETTER_VERBOSE(allocFlags, VkBufferUsageFlags);
		SETTER_VERBOSE(memoryUsage, VmaMemoryUsage);
		SETTER_VERBOSE(dynamicAccess, bool);

		BufferConfig	&baseCount(uint32_t count)
			{ _count = count; return *this; }
		BufferConfig	&fixedCount(uint32_t count)
			{ _count = count; _fixedCount = true; return *this; }

	private:
		VkBufferUsageFlags			_usage;
		VmaAllocationCreateFlags	_allocFlags;
		VmaMemoryUsage				_memoryUsage{VMA_MEMORY_USAGE_AUTO};
		uint32_t					_count{0};
		bool						_dynamicAccess{false};
		bool						_fixedCount{false};

		VkDescriptorType	_descriptorType{VK_DESCRIPTOR_TYPE_MAX_ENUM};

	friend class Buffer;
};

class	Buffer : public RefCounted {
	public:
		template <POD T>
		static expected<Ref<Buffer>>	create(Device *device,
											const BufferConfig &config);

		expected<Ref<Buffer>>	writeToBuffer(void *data, uint32_t count = 1,
											uint32_t offset = 0);

		VkDescriptorBufferInfo	getDescriptorInfo(uint32_t offset = 0);
		VkDescriptorType	getDescriptorType(void) const
			{ return _config._descriptorType; }
		VkBuffer			getBuffer(void) const
			{ return _buffer; }
		VkDeviceSize		getSize(void) const
			{ return _availableSize; }
		VkDeviceSize		getRange(void) const
			{ return _range; }
		uint32_t			getStride(void) const
			{ return _stride; }
		bool				syncVersion(uint32_t *clientVersion);

		void	*getMapped(void) const
			{ return _mapped; }
		template <POD T>
		void	*getMappedAs(void) const
			{ return static_cast<T *>(_mapped); }

	private:
		Buffer(void) = default;
		~Buffer(void);
		Buffer(const Buffer &) = delete;
		Buffer	&operator=(const Buffer &) = delete;
		Buffer(const Buffer &&) = delete;
		Buffer	&operator=(const Buffer &&) = delete;

		template <POD T>
		expected<void>	init(Device *device, const BufferConfig &config);

		void			deallocate(void);
		expected<void>	allocate(uint32_t count);

		Device			*_device;

		BufferConfig	_config;
		uint32_t		_stride;
		uint32_t		_elementSize;

		uint32_t		_maxCount{0u};
		VkDeviceSize	_availableSize{0u};
		uint32_t		_currentCount{0u};
		VkDeviceSize	_range{0u};

		VmaAllocation	_allocation{VK_NULL_HANDLE};
		VkBuffer		_buffer{VK_NULL_HANDLE};
		void			*_mapped{nullptr};

		uint32_t		_version{0};
};

}

#include "api/vulkan/Buffer.tpp"
