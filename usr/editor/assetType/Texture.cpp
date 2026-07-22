/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Texture.cpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/24 15:13:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/22 14:42:56                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "assetType/Texture.hpp"
#include "utils/VFS.hpp"
#include "api/vulkan/Image.hpp"
#include "api/vulkan/Device.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <assetLoader/stb_image.h>
#include <iostream>

namespace	hel {

Texture::RawTexture	Texture::loadFile(const std::string &path) {
	RawTexture	raw{};
	auto	realPath = VFS::getFilepath(path);
	raw.pixels = stbi_load(realPath.c_str(), &raw.width, &raw.height,
							&raw.channels, STBI_rgb_alpha);
	return (raw);
}

std::shared_ptr<Texture> Texture::load(Device *device,
											const std::string &path) {
	auto	raw = loadFile(path);
	if (!raw.pixels) {
		std::cerr << "Failed to load the image " << path << std::endl;
		return (nullptr);
	}

	auto	asset = std::make_shared<Texture>();
	asset->filePath = path;

	asset->image = Image::create(device, ImageConfig2D()
						.extent().width((uint32_t)raw.width)
						.extent().height((uint32_t)raw.height)
						.formats<VK_FORMAT_R8G8B8A8_SRGB>()
						.usage<VK_IMAGE_USAGE_TRANSFER_DST_BIT |
								VK_IMAGE_USAGE_SAMPLED_BIT>());

	uint32_t	size = static_cast<uint32_t>(raw.width * raw.height);
	auto	commandBuffer = device->beginSingleTimeCommand();
	asset->image->setData(commandBuffer, std::vector<unsigned char>(raw.pixels, raw.pixels + size));
	device->endSingleTimeCommand(commandBuffer);

	stbi_image_free(raw.pixels);

	return (asset);
}

}
