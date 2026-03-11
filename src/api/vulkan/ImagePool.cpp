/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/11 10:59:47 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/11 11:55:52                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/ImagePool.hpp"
#include "utils/mathUtils.hpp"

# include <ranges>

namespace	hel {

size_t	ImageDescHasher::operator()(const ImageDesc &desc) const {
	size_t	seed = 0;
	hel::mathUtils::hashCombine(seed, desc.format, desc.usage,
						desc.extent.height, desc.extent.width);
	return (seed);
}

bool	ImageDesc::operator==(const ImageDesc &other) const {
	return (this->format == other.format &&
			this->usage == other.usage &&
			this->extent.height == other.extent.height &&
			this->extent.width == other.extent.width);
}

ImagePool::Builder::Builder(Device &device)
	:	_device{device} {
}

ImagePool::Builder	&ImagePool::Builder::addImage(VkFormat format,
												VkExtent2D extent,
												VkImageUsageFlags usage,
												uint32_t count) {
	ImageDesc	desc = {.format = format, .extent = extent, .usage = usage};
	if (_imageDescs.find(desc) == _imageDescs.end())
		_imageDescs[desc] = 0;
	_imageDescs[desc] += count;
	return (*this);
}

std::unique_ptr<ImagePool>	ImagePool::Builder::build(void) {
	return (std::unique_ptr<ImagePool>(new ImagePool(_device, std::move(_imageDescs))));
}

}
