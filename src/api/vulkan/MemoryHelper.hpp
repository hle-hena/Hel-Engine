/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: MemoryHelper.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/04 18:37:06 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/05 11:50:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>

# include "api/vulkan/Device.hpp"

namespace	hel::MemoryHelper {

static inline VkResult	allocate(Device &device, VkMemoryRequirements requirements,
					VkMemoryPropertyFlags properties, VkDeviceMemory &memory) {
	uint32_t	typeIndex;
	if (device.findMemoryType(requirements.memoryTypeBits, properties, typeIndex))
		return (VK_ERROR_FEATURE_NOT_PRESENT);

	VkMemoryAllocateInfo	allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = requirements.size;
	allocateInfo.memoryTypeIndex = typeIndex;

	return (vkAllocateMemory(device.getLogical(), &allocateInfo, nullptr, &memory));
}

}
