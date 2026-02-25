/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/25 13:15:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/25 17:14:58                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Image.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/MemoryHelper.hpp"

namespace	hel {

Image::Image(Device &device, const Config &config)
	:	_device{device} {
	VkImageCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.format = config.format;
	createInfo.extent.width = config.width;
	createInfo.extent.height = config.height;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = config.usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(_device.getLogical(), &createInfo, nullptr, &_image))
		throw std::runtime_error("Failed to create an Image");

	VkMemoryRequirements	memRequirements;
	vkGetImageMemoryRequirements(_device.getLogical(), _image,
								&memRequirements);
	if (MemoryHelper::allocate(_device, memRequirements,
							config.properties, _memory))
		throw std::runtime_error("Failed to allocate memory for the image");
	vkBindImageMemory(_device.getLogical(), _image, _memory, 0);

	VkImageViewCreateInfo	viewCreateInfo{};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = _image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = config.format;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.subresourceRange.aspectMask = config.aspectFlags;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;
	if (vkCreateImageView(_device.getLogical(), &viewCreateInfo,
						nullptr, &_view))
		throw std::runtime_error("Failed to create the image view");
}

}
