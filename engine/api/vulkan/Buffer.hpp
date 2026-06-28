/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/29 16:04:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/25 10:44:51                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <memory>
# include <vma/vk_mem_alloc.h>

namespace	hel {

class	Device;

class Buffer {
	public:
		static std::unique_ptr<Buffer>	create(Device &device, uint32_t stride,
						uint32_t count, VkBufferUsageFlags usage,
						VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
						VmaAllocationCreateFlags allocFlags = 0);
		~Buffer(void);

		Buffer(const Buffer &) = delete;
		Buffer	operator=(const Buffer &) = delete;

		VkResult		map(void);
		void			unmap(void);

		void			writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE,
									VkDeviceSize offset = 0);
		VkResult		flush(VkDeviceSize size = VK_WHOLE_SIZE,
							VkDeviceSize offset = 0);

		VkDescriptorBufferInfo	getDescriptorInfo(void) const
			{ return {_buffer, 0, _stride}; }
		VkBuffer				getBuffer(void) const
			{ return (_buffer); }
		void					*getMapped(void) const
			{ return (_mapped); }
		VkDeviceSize			getSize(void) const
			{ return (_size); }
		uint32_t				getStride(void) const
			{ return (_stride); }
		VkDeviceSize			getOffset(void) const
			{ return (0); }

	private:
		Buffer(Device &device, uint32_t stride,
				uint32_t count, VkBufferUsageFlags usage,
				VmaMemoryUsage memoryUsage,
				VmaAllocationCreateFlags allocFlags);

		Device						&_device;
		VkBuffer					_buffer{VK_NULL_HANDLE};
		VmaAllocation				_allocation{VK_NULL_HANDLE};
		VkDeviceSize				_size;
		uint32_t					_stride;
		VmaAllocationCreateFlags	_allocFlags;
		void			*_mapped{nullptr};
};

}
