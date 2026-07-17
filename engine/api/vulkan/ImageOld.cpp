/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/25 13:15:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 17:43:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/ImageOld.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Buffer.hpp"
#include "platform/ui/UiContext.hpp"
#include "utils/mathUtils.hpp"
#include "api/vulkan/Sampler.hpp"

#include <iostream>

namespace	hel {

bool	ViewConfig::operator==(const ViewConfig &o) const {
	return (this->_aspect == o._aspect && this->_format == o._format
		&& this->_components.r == o._components.r
		&& this->_components.b == o._components.b
		&& this->_components.b == o._components.b
		&& this->_components.a == o._components.a);
}

size_t	ViewConfigHasher::operator()(const ViewConfig &conf) const {
	size_t	hash = 0;
	mathUtils::hashCombine(hash, conf._aspect, conf._format, conf._components.r,
		conf._components.g, conf._components.b, conf._components.a);
	return hash;
}


size_t	Image::ConfigHasher::operator()(const Config &conf) const {
	size_t	seed = 0;
	for (auto format: conf.format)
		hel::mathUtils::hashCombine(seed, format);
	hel::mathUtils::hashCombine(seed, conf.aspectFlags, conf.usage,
						conf.height, conf.width);
	return (seed);
}

bool	Image::Config::operator==(const Config &other) const {
	return (this->format == other.format &&
			this->usage == other.usage &&
			this->width == other.width &&
			this->height == other.height &&
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
	createImage();
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
	_extent = extent;
}

Image::~Image(void) {
	for (auto &[conf, text]: _textures)
		UiContext::unregisterTexture(text);
	for (auto &[conf, view]: _views)
		vkDestroyImageView(_device.getLogical(), view, nullptr);
	if (_owned && _image)
		vmaDestroyImage(_device.getAllocator(), _image, _allocation);
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

	VmaAllocationCreateInfo	allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	if (vmaCreateImage(_device.getAllocator(), &createInfo, &allocCreateInfo,
					&_image, &_allocation, nullptr))
		throw std::runtime_error("Failed to create an Image");
}

VkImageView	Image::createView(const ViewConfig &conf) {
	if (conf._aspect == 0 || conf._aspect == VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM)
		return VK_NULL_HANDLE;
	if (conf._format == VK_FORMAT_MAX_ENUM)
		return VK_NULL_HANDLE;
	if (conf._components.r == VK_COMPONENT_SWIZZLE_MAX_ENUM
		|| conf._components.g == VK_COMPONENT_SWIZZLE_MAX_ENUM
		|| conf._components.b == VK_COMPONENT_SWIZZLE_MAX_ENUM
		|| conf._components.a == VK_COMPONENT_SWIZZLE_MAX_ENUM)
		return VK_NULL_HANDLE;

	VkImageView				view{VK_NULL_HANDLE};
	VkImageViewCreateInfo	viewCreateInfo{};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = _image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = conf._format;
	viewCreateInfo.components.r = conf._components.r;
	viewCreateInfo.components.g = conf._components.g;
	viewCreateInfo.components.b = conf._components.b;
	viewCreateInfo.components.a = conf._components.a;
	viewCreateInfo.subresourceRange.aspectMask = conf._aspect;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1; //TODO -> support mipmaps
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1; //TODO -> support mipmaps
	if (vkCreateImageView(_device.getLogical(), &viewCreateInfo,
						nullptr, &view))
		return VK_NULL_HANDLE;
	_views[conf] = view;
	return view;
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

	//TODO -> rework this.
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

expected<void>	Image::setData(void *data, uint32_t count) {
	Ref<Buffer>	stagingBuffer;
	auto		res = Buffer::create<char>(&_device, BufferConfig()
					.allocFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
					.usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
					.fixedCount(count))
		.and_then([&](Ref<Buffer> buffer) -> expected<Ref<Buffer>> {
			stagingBuffer = buffer;
			return buffer->writeToBuffer(data, count);
		});
	if (!res)
		return unexpected("Failed to write/create the buffer for image write: "
					+ res.error());

	VkCommandBuffer commandBuffer = _device.beginSingleTimeCommand();
	transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	VkBufferImageCopy	region{};
	region.imageSubresource = {_config.aspectFlags, 0, 0, 1};
	region.imageExtent = { _config.width, _config.height, 1 };
	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), _image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	_device.endSingleTimeCommand(commandBuffer);
	return {};
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
	setWrittenState();
}

void	Image::copyTo(VkCommandBuffer commandBuffer, Ref<Buffer> dst,
					VkOffset3D startPos, VkExtent3D extent) {
	this->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	VkBufferImageCopy2	copyRegion{};
	copyRegion.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
	copyRegion.bufferOffset = 0;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;

	copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.imageSubresource.mipLevel = 0;
	copyRegion.imageSubresource.baseArrayLayer = 0;
	copyRegion.imageSubresource.layerCount = 1;

	copyRegion.imageOffset = startPos;
	copyRegion.imageExtent = extent;

	VkCopyImageToBufferInfo2	copyInfo{};
	copyInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2;
	copyInfo.dstBuffer = dst->getBuffer();
	copyInfo.srcImage = _image;
	copyInfo.srcImageLayout = _currentLayout;
	copyInfo.pRegions = &copyRegion;
	copyInfo.regionCount = 1;

	vkCmdCopyImageToBuffer2(commandBuffer, &copyInfo);
}

VkImageView	Image::getView(const ViewConfig &conf) {
	if (_views.contains(conf))
		return _views[conf];
	return createView(conf);
}

VkDescriptorSet	Image::getTexture(VkImageView view) {
	if (_textures.contains(view))
		return _textures[view];

	auto	texture = UiContext::registerTexture(_device, Sampler::getSampler(_device, {}), view);
	_textures[view] = texture;
	return texture;
}

VkDescriptorImageInfo	Image::getDescriptorInfo(VkImageView view) const {
	return {nullptr, view, _currentLayout};
}

VkRenderingAttachmentInfo	Image::getRenderingInfo(VkClearValue clearValue,
											VkAttachmentLoadOp loadOp,
											VkAttachmentStoreOp storeOp,
											VkImageView view) const {
	VkRenderingAttachmentInfo	info{};
	info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	info.imageView = view;
	info.imageLayout = _currentLayout;
	info.clearValue = clearValue;
	info.loadOp = loadOp;
	info.storeOp = storeOp;

	return (info);
}

}
