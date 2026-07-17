/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 15:33:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 19:47:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Image.hpp"
#include "api/vulkan/Device.hpp"

#include <set>

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

expected<Ref<Image>>	Image::wrapImage(Device *device, VkImage image,
								VkFormat format, VkExtent2D extent)
{
	Ref<Image>	newImage(new Image());
	if (auto res = newImage->init(device, image, format, extent); !res)
		return unexpected("Couldn't create an image: " + res.error());
	return newImage;
}

expected<void>	Image::init(Device *device, VkImage image, VkFormat format,
							VkExtent2D extent)
{
	_device = device;
	_config = ImageConfig2D()
				.formats({format})
				.extent().width(extent.width)
				.extent().height(extent.height);
	_config._type = VK_IMAGE_TYPE_2D;
	_config._owning = false;

	_image = image;

	return {};
}

expected<void>	Image::validateConfig(void) {
	if (!_config._owning)
		return {};

	if (_config._layers == 0)
		return unexpected("Expecting at least 1 layer.");

	if (_config._formats.empty())
		return unexpected("Expecting at least one format in the image config.");

	std::set<VkFormat>	known;
	for (auto &format: _config._formats) {
		known.insert(format);
		//TODO -> check for size mismatch of different formats.
	}
	if (known.size() != _config._formats.size())
		return unexpected("Detected duplicate format in the format list.");

	if (_config._usage == VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM
		|| _config._usage == 0)
		return unexpected("Expecting a valid image usage.");
	if (_config._allocFlags != 0)
		return unexpected("Allocation flags are not implemented yet.");

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


void	Image::transitionLayout(VkCommandBuffer commandBuffer,
								VkImageLayout newlayout)
{
		
}


}
