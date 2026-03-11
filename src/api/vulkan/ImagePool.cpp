/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/11 10:59:47 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/11 17:09:03                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/ImagePool.hpp"
#include "utils/mathUtils.hpp"

namespace	hel {

ImagePool::Builder::Builder(Device &device)
	:	_device{device} {
}

ImagePool::Builder	&ImagePool::Builder::addImage(const Image::Config &config,
												uint32_t count) {
	if (_imageDescs.find(config) == _imageDescs.end())
		_imageDescs[config] = 0;
	_imageDescs[config] += count;
	return (*this);
}

std::unique_ptr<ImagePool>	ImagePool::Builder::build(void) {
	return (std::unique_ptr<ImagePool>(new ImagePool(_device, std::move(_imageDescs))));
}



ImagePool::ImagePool(Device &device, ImageDescMap<uint32_t> &&imageDescs)
	:	_device{device} {
	for (auto &it: imageDescs) {
		for (auto i = 0; i < it.second; i++) {
			auto	&slot = _pools[it.first].emplace_back();
			slot.image = Image::create(_device, it.first);
		}
	}
}

ImagePool::~ImagePool(void) {
}

Image	*ImagePool::acquire(const Image::Config &config) {
	auto	it = _pools.find(config);
	if (it == _pools.end())
		return (nullptr);
	for (auto &slot: it->second) {
		if (!slot.inUse) {
			slot.inUse = true;
			return (slot.image.get());
		}
	}
	return (nullptr);
}

void	ImagePool::release(Image *image) {
	for (auto &pool: _pools) {
		for (auto &slot: pool.second) {
			if (slot.image.get() == image) {
				slot.inUse = false;
				return ;
			}
		}
	}
}

}
