/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.tpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 17:52:36 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/23 11:53:36                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Image.hpp"
#include "utils/Logger.hpp"

namespace	hel {

template <VkFormat Format>
Ref<Image>	Image::wrapImage(Device *device, VkImage image,
										VkExtent2D extent)
{
	Ref<Image>	newImage(new Image());
	if (auto res = newImage->init<Format>(device, image, extent); !res) {
		HEL_FATAL("Couldn't wrap an image: {}", res.error());
		return nullptr;
	}
	return newImage;
}

template <VkFormat Format>
expected<void>	Image::init(Device *device, VkImage image, VkExtent2D extent) {
	_device = device;
	_config = ImageConfig2D()
				.formats<Format>()
				.extent().width(extent.width)
				.extent().height(extent.height);
	_config._owning = false;

	_image = image;
	_extent = _config._extent;

	return {};
}

}
