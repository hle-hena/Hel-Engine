/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Texture.cpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/24 15:13:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/24 17:35:51                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/assets/Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "api/image/stb_image.h"

#include <iostream>

namespace	hel {

Texture::RawTexture	Texture::loadFile(const std::string &path) {
	RawTexture	raw{};
	raw.pixels = stbi_load(path.c_str(), &raw.width, &raw.height,
							&raw.channels, STBI_rgb_alpha);
	return (raw);
}

std::shared_ptr<Texture> Texture::load(Device &device,
											const std::string &path) {
	auto	raw = loadFile(path);
	if (!raw.pixels) {
		std::cerr << "Failed to load the image " << path << std::endl;
		return (nullptr);
	}

	auto	asset = std::make_shared<Texture>();
	asset->filePath = path;

	asset->image = Image::create(device,
		Image::Config{}
			.setWidth(raw.width)
			.setHeight(raw.height)
			.setFormats({VK_FORMAT_R8G8B8A8_SRGB})
			.setUsage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
			.setProperty(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			.setAspect(VK_IMAGE_ASPECT_COLOR_BIT));

	VkDeviceSize	size = raw.width * raw.height * 4;
	asset->image->setData(raw.pixels, size);

	stbi_image_free(raw.pixels);
	return (asset);
}

}
