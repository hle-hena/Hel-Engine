/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 15:33:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/21 15:44:40                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Image.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Buffer.hpp"
#include "platform/ui/UiContext.hpp"
#include "api/vulkan/Sampler.hpp"

namespace	hel {

Image::~Image(void) {
	deallocateImage();
}

void			Image::deallocateImage(void) {
	if (_allocation && _config._owning)
		vmaDestroyImage(_device->getAllocator(), _image, _allocation);
	_image = VK_NULL_HANDLE;
	_allocation = VK_NULL_HANDLE;
}

expected<void>	Image::validateConfig(void) {
	if (!_config._owning)
		return {};

	if (_config._allocFlags != 0)
		return unexpected("Allocation flags are not implemented yet.");

	if (_config._layers == 0)
		return unexpected("Expecting at least 1 layer.");

	if (_config._extent.width == 0)
		return unexpected("The width of an image cannot be 0.");
	if (_config._extent.height == 0)
		return unexpected("The height of an image cannot be 0.");
	if (_config._extent.depth == 0)
		return unexpected("The depth of an image cannot be 0.");

	return {};
}

expected<void>	Image::allocateImage(void) {
	if (!_config._owning)
		return {};

	VkImageCreateInfo		create{};
	create.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	create.imageType = _config._type;
	create.usage = _config._usage;
	create.extent = _config._extent;
	create.arrayLayers = _config._layers;
	create.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	create.mipLevels = 1;//TODO -> handle mipmapping.
	create.samples = VK_SAMPLE_COUNT_1_BIT;//TODO -> handle anti aliasing.
	create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create.tiling = VK_IMAGE_TILING_OPTIMAL;

	create.format = _config._formats[0];
	VkImageFormatListCreateInfo	formatList{};
	uint32_t	formatCount = static_cast<uint32_t>(_config._formats.size());
	if (formatCount > 1) {
		formatList.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;
		formatList.viewFormatCount = formatCount;
		formatList.pViewFormats = _config._formats.data();
		create.pNext = &formatList;
		create.flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	}

	VmaAllocationCreateInfo	allocCreate{};
	allocCreate.usage = VMA_MEMORY_USAGE_AUTO;
	if (vmaCreateImage(_device->getAllocator(), &create, &allocCreate,
						&_image, &_allocation, nullptr) != VK_SUCCESS)
		return unexpected("Failed to allocate an image.");
	return {};
}

expected<VkImageView>	Image::createView(const ViewConfig &conf) {
	if (std::find(_config._formats.begin(), _config._formats.end(),
					conf._format) == _config._formats.end())
		return unexpected("Cannot create a view on a format not defined at "
					"image creation.");
	if (conf._components.r == VK_COMPONENT_SWIZZLE_MAX_ENUM
		|| conf._components.g == VK_COMPONENT_SWIZZLE_MAX_ENUM
		|| conf._components.b == VK_COMPONENT_SWIZZLE_MAX_ENUM
		|| conf._components.a == VK_COMPONENT_SWIZZLE_MAX_ENUM)
		return unexpected("Cannot create a view with one of the components as "
						"VK_COMPONENT_SWIZZLE_MAX_ENUM.");

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
	viewCreateInfo.subresourceRange.aspectMask = _aspect;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1; //TODO -> support mipmaps
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1; //TODO -> support mipmaps
	if (vkCreateImageView(_device->getLogical(), &viewCreateInfo,
						nullptr, &view))
		return unexpected("Couldn't create the view.");
	_views[conf] = view;
	return view;
}





expected<VkImageView>	Image::getView(const ViewConfig &conf) {
	if (auto it = _views.find(conf); it != _views.end())
		return it->second;
	return createView(conf);
}

VkDescriptorSet	Image::getTexture(VkImageView view) {
	if (_textures.contains(view))
		return _textures[view];

	auto	texture = UiContext::registerTexture(*_device,
						Sampler::getSampler(*_device, {}), view);
	_textures[view] = texture;
	return texture;
}

VkDescriptorImageInfo	Image::getDescriptorInfo(VkImageView view) const {
	return {nullptr, view, _currentLayout};
}

VkRenderingAttachmentInfo	Image::getRenderingInfo(VkClearValue clearValue,
											VkAttachmentLoadOp loadOp,
											VkAttachmentStoreOp storeOp,
											VkImageView view) const
{
	VkRenderingAttachmentInfo	info{};
	info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	info.imageView = view;
	info.imageLayout = _currentLayout;
	info.clearValue = clearValue;
	info.loadOp = loadOp;
	info.storeOp = storeOp;

	return (info);
}

void	Image::transitionLayout(VkCommandBuffer commandBuffer,
								VkImageLayout newlayout)
{
	if (newlayout == _currentLayout)
		return ;

	VkImageMemoryBarrier2	barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.image = _image;
	barrier.subresourceRange = {};
	barrier.subresourceRange.aspectMask = _aspect;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = _config._layers;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;

	//Both will be fixed with a render graph
	//TODO -> bookkeeping the image's access and stages, to optimize this.
	barrier.srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT
							| VK_ACCESS_2_MEMORY_READ_BIT;
	barrier.newLayout = newlayout;

	//TODO -> explicit access and stage usage to optimize this.
	barrier.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT
							| VK_ACCESS_2_MEMORY_READ_BIT;
	barrier.oldLayout = _currentLayout;

	VkDependencyInfo	dep;
	dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dep.imageMemoryBarrierCount = 1;
	dep.pImageMemoryBarriers = &barrier;
	vkCmdPipelineBarrier2(commandBuffer, &dep);
	_currentLayout = newlayout;
}

expected<void>	Image::validateSetData(const std::vector<char *> &src) {
	if (src.empty())
		return unexpected("The src was empty.");
	if (!(_config._usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
		return unexpected("The dst image of a copy must have the transfer "
						"dst usage bit.");
	return {};
}

expected<void>	Image::setData(VkCommandBuffer commandBuffer,
							const std::vector<char *> &src)
{
	if (auto r = validateSetData(src); !r)
		return unexpected("Error when copying from a buffer: " + r.error());

	Ref<Buffer>	stagingBuffer;
	auto		count = static_cast<uint32_t>(src.size());
	auto		data = static_cast<const void *>(src.data());
	auto		res = Buffer::create<char>(_device, BufferConfig()
					.allocFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
					.usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
					.fixedCount(count))
		.and_then([&](Ref<Buffer> buffer) -> expected<Ref<Buffer>> {
			stagingBuffer = buffer;
			return buffer->writeToBuffer(data, count);
		});
	if (!res)
		return unexpected("Failed to write/create the buffer: "
					+ res.error());

	transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	VkBufferImageCopy	region{};
	region.imageSubresource.aspectMask = _aspect;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.mipLevel = 1;
	region.imageExtent = _config._extent;
	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), _image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	return {};
}

expected<void>	Image::validateCopy(Ref<Image> dst) {
	if (!dst)
		return unexpected("The dst image is a nullptr.");

	auto	srcUsage = _config._usage;
	auto	dstUsage = dst->_config._usage;
	auto	srcAspect = _aspect;
	auto	dstAspect = dst->_aspect;
	auto	srcFormat = _config._formats[0];
	auto	dstFormat = dst->_config._formats[0];

	if (!(srcUsage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT))
		return unexpected("The src image of a copy must have the transfer "
						"src usage bit.");
	if (!(dstUsage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
		return unexpected("The dst image of a copy must have the transfer "
						"dst usage bit.");

	if (srcAspect != dstAspect)
		return unexpected("The two images' aspect must match.");
	if (srcAspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) {
		if (srcFormat != dstFormat)
			return unexpected("To copy a depth/stencil image, the format of "
						"both images must match.");
		return {};
	}

	if (!isSameDataType(srcFormat, dstFormat))
		return unexpected("The two images' must have the same data type "
						"(sint, uint, sfloat, ...)");
	return {};
}

expected<void>	Image::copyTo(VkCommandBuffer commandBuffer, Ref<Image> dst) {
	if (auto res = validateCopy(dst); !res)
		return unexpected("Error when copying to an image: " + res.error());

	transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	dst->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkImageBlit2 region{};
	region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	region.srcSubresource = {_aspect, 0, 1, 1};
	region.srcOffsets[0] = {0, 0, 0};
	region.srcOffsets[1] = {(int)_config._extent.width,
							(int)_config._extent.height, 1};
	region.dstSubresource = {dst->_aspect, 0, 1, 1};
	region.dstOffsets[0] = {0, 0, 0};
	region.dstOffsets[1] = {(int)dst->_config._extent.width,
							(int)dst->_config._extent.height, 1};

	VkBlitImageInfo2 blitInfo{};
	blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	blitInfo.srcImage = _image;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.dstImage = dst->_image;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &region;
	blitInfo.filter = VK_FILTER_LINEAR;
	vkCmdBlitImage2(commandBuffer, &blitInfo);
	dst->setWrittenState();
	return {};
}

void	Image::copyTo(VkCommandBuffer commandBuffer, Ref<Buffer> dst,
					VkOffset3D startPos, VkExtent3D extent) {
	transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

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

}
