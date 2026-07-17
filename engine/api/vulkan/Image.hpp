/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 15:33:47 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 17:04:20                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <vector>

#include "utils/Setters.hpp"
#include "utils/Ref.hpp"
#include "HelExpected.hpp"

namespace	hel {

class	Device;

struct	ImageConfig {
	public:
		SETTER_VERBOSE(usage, VkImageUsageFlags)
		SETTER_VERBOSE(allocFlags, VmaAllocationCreateFlags)
		SETTER_VERBOSE(memoryUsage, VmaMemoryUsage)
		SETTER_VERBOSE(formats, std::initializer_list<VkFormat>)

	private:
		VkImageUsageFlags			_usage;
		VmaAllocationCreateFlags	_allocFlags;
		VmaMemoryUsage				_memoryUsage{VMA_MEMORY_USAGE_AUTO};
		std::vector<VkFormat>		_formats;

	friend class	Image;
};

class	Image {
	public:
		expected<Ref<Image>>	create(Device *device, const ImageConfig &config);

	private:
		void			deallocateImage(void);
		expected<void>	allocateImage(void);
		expected<void>	init(Device *device, const ImageConfig &config);

		Device			*_device;
		ImageConfig		_config;

		VmaAllocation	_allocation{VK_NULL_HANDLE};
		VkImage			_image{VK_NULL_HANDLE};
};

}
