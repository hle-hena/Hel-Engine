/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/25 13:15:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/12 16:15:02                                        */
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
#include "platform/ui/UiContext.hpp"

#include <iostream>
#include <stdexcept>

namespace	hel {

size_t	Image::ConfigHasher::operator()(const Config &conf) const {
	size_t	seed = 0;
	for (auto format: conf.format)
		hel::mathUtils::hashCombine(seed, format);
	hel::mathUtils::hashCombine(seed, conf.aspectFlags, conf.usage,
						conf.height, conf.width, conf.properties);
	return (seed);
}

bool	Image::Config::operator==(const Config &other) const {
	return (this->format == other.format &&
			this->usage == other.usage &&
			this->width == other.width &&
			this->height == other.height &&
			this->properties == other.properties &&
			this->aspectFlags == other.aspectFlags);
}


std::unique_ptr<Image>	Image::create(Device &device, const Config &config) {
	try {
		return (std::unique_ptr<Image>(new Image(device, config)));
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (nullptr);
	}
}

std::unique_ptr<Image>	Image::wrapSwapchainImages(Device &device, VkImage img,
										VkFormat format, VkExtent2D extent) {
	try {
		return (std::unique_ptr<Image>(new Image(device, img,
											format, extent)));
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (nullptr);
	}
}

Image::Image(Device &device, const Config &config)
	:	_device{device},
		_config{config} {
	createImage(), allocateMemory(), createViews();
	_extent = {config.width, config.height};
}

Image::Image(Device &device, VkImage img, VkFormat format, VkExtent2D extent)
	:	_device{device} {
	_image = img;
	_owned = false;
	_config.format = {format};
	_config.width = extent.width;
	_config.height = extent.height;
	_config.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	_config.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	createViews();
	_extent = extent;
}

Image::~Image(void) {
	for (auto it: _textures)
		UiContext::unregisterTexture(it.second);
	for (auto it: _views)
		vkDestroyImageView(_device.getLogical(), it.second, nullptr);
	if (_owned && _memory)
		vkFreeMemory(_device.getLogical(), _memory, nullptr);
	if (_owned && _image)
		vkDestroyImage(_device.getLogical(), _image, nullptr);
}

void	Image::createImage(void) {
	VkImageCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.flags = _config.format.size() == 1 ? 0 :
							VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.format = _config.format[0];
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

void	Image::createViews(void) {
	static constexpr VkImageUsageFlags	viewFlags = VK_IMAGE_USAGE_SAMPLED_BIT |
		VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	if (!(viewFlags & _config.usage))
		return ;
	for (auto i = 0; i < _config.format.size(); i++)
		createView(_config.format[i]);
}

void	Image::createView(VkFormat format) {
	VkImageViewCreateInfo	viewCreateInfo{};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = _image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = format;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.subresourceRange.aspectMask = _config.aspectFlags;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1; //TODO -> support mipmaps
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1; //TODO -> support mipmaps
	if (vkCreateImageView(_device.getLogical(), &viewCreateInfo,
						nullptr, &_views[format]))
		throw std::runtime_error("Failed to create an image view");
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

void	Image::transitionLayout(VkCommandBuffer commandBuffer,
								VkImageLayout newLayout) {
	if (newLayout == _currentLayout)
		return ;
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
	_currentLayout = newLayout;
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

void	Image::copyTo(VkCommandBuffer commandBuffer, Image *dst) {
	this->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	dst->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkImageBlit2 region{};
	region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	region.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
	region.srcOffsets[0] = {0, 0, 0};
	region.srcOffsets[1] = {(int)this->_config.width,
							(int)this->_config.height, 1};
	region.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
	region.dstOffsets[0] = {0, 0, 0};
	region.dstOffsets[1] = {(int)dst->_config.width,
							(int)dst->_config.height, 1};

	VkBlitImageInfo2 blitInfo{};
	blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	blitInfo.srcImage = this->_image;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.dstImage = dst->_image;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &region;
	blitInfo.filter = VK_FILTER_LINEAR;
	vkCmdBlitImage2(commandBuffer, &blitInfo);
}

VkDescriptorSet	Image::getTexture(VkFormat format) {
	if (_textures.find(format) != _textures.end())
		return	 (_textures.at(format));
	//TODO -> The image itself probably shouldn't own that ?
	//      Or just allocate the set
	_textures[format] = UiContext::registerTexture(_device, this, format);
	return (_textures[format]);
}

VkDescriptorImageInfo	Image::getDescriptorInfo(VkFormat format) const {
	return {nullptr, _views.at(format), _currentLayout};
}

VkRenderingAttachmentInfo	Image::getRenderingInfo(VkClearValue clearValue,
													VkAttachmentLoadOp loadOp,
													VkAttachmentStoreOp storeOp,
													VkFormat format) const {
	VkRenderingAttachmentInfo	info{};
	info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	info.imageView = _views.at(format);
	info.imageLayout = _currentLayout;
	info.clearValue = clearValue;
	info.loadOp = loadOp;
	info.storeOp = storeOp;

	return (info);
}

}
