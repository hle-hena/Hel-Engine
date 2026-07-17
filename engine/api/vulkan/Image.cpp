/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 15:33:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 17:10:47                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Image.hpp"
#include "api/vulkan/Device.hpp"

namespace	hel {

expected<Ref<Image>>	Image::create(Device *device,
									const ImageConfig &config)
{
	auto	newImage = makeRef<Image>();

	if (auto res = newImage->init(device, config); !res)
		return unexpected("Couldn't create an image: " + res.error());

}

expected<void>	Image::init(Device *device, const ImageConfig &config) {
	_device = device;
	_config = config;

	if (auto res = allocateImage(); !res)
		return unexpected(res.error());

	return {};
}

void			Image::deallocateImage(void) {
	if (_allocation)
		vmaDestroyImage(_device->getAllocator(), _image, _allocation);
	_image = VK_NULL_HANDLE;
	_allocation = VK_NULL_HANDLE;
}

expected<void>	Image::allocateImage(void) {
	VkImageCreateInfo		create{};
	create.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	create.imageType = VK_IMAGE_TYPE_2D;
	//TODO -> in the future, maybe create different class derived from this one
	//			for different image type.
	create.usage = _config._usage;
	create.arrayLayers = 1;

	VkImageFormatListCreateInfo	formatList{};
	if (_config._formats.empty())
		return unexpected("Expecting at least one format in the image config.");
	create.format = _config._formats[0];
	if (_config._formats.size() > 1) {
		formatList.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;
		formatList.viewFormatCount = static_cast<uint32_t>(_config._formats.size());
		formatList.pViewFormats = _config._formats.data();
		create.pNext = &formatList;
		create.flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	}

	VmaAllocationCreateInfo	allocCreate{};
	if (vmaCreateImage(_device->getAllocator(), &create, &allocCreate,
						&_image, &_allocation, nullptr) != VK_SUCCESS)
		return unexpected("Failed to allocate an image.");
	return {};
}


}
