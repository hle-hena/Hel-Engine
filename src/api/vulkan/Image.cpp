/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/25 13:15:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/25 18:48:15                                        */
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
#include "api/vulkan/Buffer.hpp"

#include <iostream>
#include <stdexcept>

namespace	hel {

std::unique_ptr<Image>	Image::create(Device &device, const Config &config) {
	try {
		return (std::unique_ptr<Image>(new Image(device, config)));
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (nullptr);
	}
}

Image::Image(Device &device, const Config &config)
	:	_device{device},
		_config{config} {
	createImage(), allocateMemory(), createView();
}

Image::~Image(void) {
	if (_view)
		vkDestroyImageView(_device.getLogical(), _view, nullptr);
	if (_memory)
		vkFreeMemory(_device.getLogical(), _memory, nullptr);
	if (_image)
		vkDestroyImage(_device.getLogical(), _image, nullptr);
}

void	Image::createImage(void) {
	VkImageCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.format = _config.format;
	createInfo.extent.width = _config.width;
	createInfo.extent.height = _config.height;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = _config.usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(_device.getLogical(), &createInfo, nullptr, &_image))
		throw std::runtime_error("Failed to create an Image");
}

void	Image::createView(void) {
	VkImageViewCreateInfo	viewCreateInfo{};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = _image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = _config.format;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.subresourceRange.aspectMask = _config.aspectFlags;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;
	if (vkCreateImageView(_device.getLogical(), &viewCreateInfo,
						nullptr, &_view))
		throw std::runtime_error("Failed to create the image view");
}

void	Image::allocateMemory(void) {
	VkMemoryRequirements	memRequirements;
	vkGetImageMemoryRequirements(_device.getLogical(), _image,
								&memRequirements);
	if (MemoryHelper::allocate(_device, memRequirements,
							_config.properties, _memory))
		throw std::runtime_error("Failed to allocate memory for the image");
	vkBindImageMemory(_device.getLogical(), _image, _memory, 0);
}

bool	Image::transitionLayout(VkCommandBuffer commandBuffer,
								VkImageLayout newLayout) {
	VkImageMemoryBarrier2	barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT |
							VK_ACCESS_2_MEMORY_WRITE_BIT;
	barrier.oldLayout = _currentLayout;
	barrier.newLayout = newLayout;
	barrier.image = _image;
	barrier.subresourceRange = {_config.aspectFlags, 0, 1, 0, 1};

	if (_currentLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;
		barrier.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
	} else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}

	VkDependencyInfo	depInfo{};
	depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &barrier;
	vkCmdPipelineBarrier2(commandBuffer, &depInfo);
}

void	Image::setData(void *data, VkDeviceSize size) {
	auto	stagingBuffer = Buffer::create(_device, size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer->map();
	stagingBuffer->writeToBuffer(data);
	stagingBuffer->unmap();

	VkCommandBuffer commandBuffer = _device.beginSingleTimeCommand();
	transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	VkBufferImageCopy	region{};
	region.imageSubresource = {_config.aspectFlags, 0, 0, 1};
	region.imageExtent = { _config.width, _config.height, 1 };
	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), _image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	_device.endSingleTimeCommand(commandBuffer);
}

}
