/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/11 10:59:47 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/23 15:59:06                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/ImagePool.hpp"

#include <algorithm>

namespace	hel {

	
Ref<ImagePool>	ImagePool::create(Device *device) {
	return new ImagePool(device);
}

ImagePool::~ImagePool(void) {
}

ImagePool::ImagePool(Device *device) {
	_device = device;
}

Ref<Image>	ImagePool::acquire(uint32_t frameIndex,
							const ImageInfo &requestedConfig)
{
	auto	&slots = _pools[requestedConfig];

	if (slots.unusedImages.empty()) {
		auto	image = Image::create(_device, requestedConfig);
		slots.usedImages[frameIndex].push_back(image);
		return image;
	} else {
		auto	entry = slots.unusedImages.back();
		slots.unusedImages.pop_back();
		slots.usedImages[frameIndex].push_back(entry.image);
		return entry.image;
	}
}

void	ImagePool::collectFromFrame(uint32_t frameIndex) {
	for (auto &[info, slots]: _pools) {
		auto	&used = slots.usedImages[frameIndex];
		for (size_t i = 0; i < used.size();) {
			if (used[i].refCount() == 1) {
				slots.unusedImages.push_back({used[i]});
				used[i] = std::move(used.back());
				used.pop_back();
			} else	{ ++i; }
		}
	}
}

void	ImagePool::evict(void) {
	auto	now = clock::now();
	for (auto &[info, slots]: _pools) {
		auto	&v = slots.unusedImages;
		auto	it = std::find_if(v.begin(), v.end(), [&](UnusedEntry &entry){
			return (now - entry.releaseAt) < evictionThreshold;
		});
		v.erase(v.begin(), it);
	}
}

}
