/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.tpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 17:52:36 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 18:18:20                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Image.hpp"

namespace	hel {

template <ImageType T>
expected<Ref<Image>>	Image::create(Device *device,
									const ImageConfig<T> &config)
{
	auto	newImage = makeRef<Image>();

	if (auto res = newImage->init(device, config); !res)
		return unexpected("Couldn't create an image: " + res.error());

}

template <ImageType T>
expected<void>	Image::init(Device *device, const ImageConfig<T> &config) {
	_device = device;
	_config = config;

	if constexpr (std::is_same_v<T, ImageTypeCube>) {
		_config._layers = 6;
		_config._usage |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}
	_config._type = T::imageType;
		
	if (auto res = validateConfig().and_then([this](void)
						{return allocateImage();}); !res)
		return unexpected(res.error());
	return {};
}

}
