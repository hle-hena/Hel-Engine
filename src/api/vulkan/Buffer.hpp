/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Buffer.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/29 16:04:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/29 16:44:31                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>

namespace	hel {

class	Device;

class Buffer {
	public:
		Buffer(Device &device, VkDeviceSize size, VkBufferUsageFlags usage,
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

		VkBuffer		getBuffer() const { return _buffer; }
		VkDeviceSize	getSize() const { return _size; }

	private:
		uint32_t	findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		Device			&_device;
		VkBuffer		_buffer{VK_NULL_HANDLE};
		VkDeviceMemory	_memory{VK_NULL_HANDLE};
		VkDeviceSize	_size;
		void			*_mapped{nullptr};
};

}
