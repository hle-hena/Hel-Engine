/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/29 16:04:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/23 19:09:38                                        */
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

namespace	hel {

class	Device;

class Buffer {
	public:
		static std::unique_ptr<Buffer>	create(Device &device, uint32_t stride,
						uint32_t count, VkBufferUsageFlags usage,
						VkMemoryPropertyFlags properties);
		~Buffer(void);

		Buffer(const Buffer &) = delete;
		Buffer	operator=(const Buffer &) = delete;

		VkResult		map(VkDeviceSize size = VK_WHOLE_SIZE,
							VkDeviceSize offset = 0);
		void			unmap();

		void			writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE,
									VkDeviceSize offset = 0);
		VkResult		flush(VkDeviceSize size = VK_WHOLE_SIZE,
							VkDeviceSize offset = 0);

		VkDescriptorBufferInfo	getDescriptorInfo(void) const
			{ return {_buffer, 0, _stride}; }
		VkBuffer				getBuffer(void) const
			{ return (_buffer); }
		VkDeviceSize			getSize(void) const
			{ return (_size); }
		VkDeviceSize			getOffset(void) const
			{ return (0); }

	private:
		Buffer(Device &device, uint32_t stride,
				uint32_t count, VkBufferUsageFlags usage,
				VkMemoryPropertyFlags properties);

		Device			&_device;
		VkBuffer		_buffer{VK_NULL_HANDLE};
		VkDeviceMemory	_memory{VK_NULL_HANDLE};
		VkDeviceSize	_size;
		uint32_t		_stride;
		void			*_mapped{nullptr};
};

}
