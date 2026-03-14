/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/11 10:59:47 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/14 17:49:26                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/ImagePool.hpp"

#include <algorithm>
#include <bit>
#include <iostream>

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

bool	ImagePool::candidateFits(const Image::Config &requested,
								const Image::Config &candidate) {
	bool	sizeFit = requested.width <= candidate.width &&
			requested.height <= candidate.height;
	bool	usageFit = (requested.usage & candidate.usage) == requested.usage;
	bool	propertiesFit = (requested.properties & candidate.properties)
													== requested.properties;
	bool	aspectFit = (requested.aspectFlags & candidate.aspectFlags)
													== requested.aspectFlags;
	bool	formatFit = true;
	for (auto fmt: requested.format) {
		if (std::find(candidate.format.begin(), candidate.format.end(), fmt)
													== candidate.format.end())
			formatFit = false;
	}

	return (sizeFit && usageFit && propertiesFit && aspectFit);
}

uint64_t	ImagePool::candidateScore(const Image::Config &requested,
									const Image::Config &candidate) {
	uint64_t	waste = 0;

	waste += (candidate.width - requested.width) *
			(candidate.height - requested.height) * 100;
	waste += std::popcount(candidate.usage & ~requested.usage);
	waste += std::popcount(candidate.properties & ~requested.properties);
	waste += std::popcount(candidate.aspectFlags & ~requested.aspectFlags);
	for (auto fmt: candidate.format) {
		if (std::find(requested.format.begin(), requested.format.end(), fmt) == requested.format.end())
			waste += 1;
	}
	return (waste);
}

auto	ImagePool::findNamed(const std::string &referenceID) {
	return (std::find_if(_namedImages.begin(), _namedImages.end(),
								[&](const auto &pair){
									return (pair.first == referenceID);
								}));
}

auto	ImagePool::findNamed(Image *image) {
	return (std::find_if(_namedImages.begin(), _namedImages.end(),
								[&](const auto &pair){
									return (pair.second == image);
								}));
}

Image	*ImagePool::acquire(const Image::Config &requested) {
	Slot		*bestSlot = nullptr;
	uint64_t	bestScore = UINT64_MAX;

	for (auto &[candidate, pool]: _pools) {
		if (!candidateFits(requested, candidate))
			continue ;
		uint64_t	score = candidateScore(requested, candidate);
		if (score >= bestScore)
			continue ;
		for (auto &slot: pool) {
			if (slot.inUse)
				continue ;
			bestSlot = &slot;
			bestScore = score;
			break ;
		}
	}
	if (!bestSlot)
		return (nullptr);
	bestSlot->inUse = true;
	bestSlot->image->setExtent({std::max(requested.width, 1u), std::max(requested.height, 1u)}, {});
	return (bestSlot->image.get());
}

Image	*ImagePool::acquire(const std::string &referenceID,
							const Image::Config &requested) {
	if (findNamed(referenceID) != _namedImages.end()) {
		std::cerr << "Name already taken" << std::endl;
		return (nullptr);
	}
	auto	image = acquire(requested);
	if (image)
		_namedImages.push_back({referenceID, image});
	return (image);
}

Image	*ImagePool::get(const std::string &referenceID) {
	auto	it = findNamed(referenceID);
	if (it != _namedImages.end())
		return (it->second);
	return (nullptr);
}

void	ImagePool::removeIfNamed(Image *image) {
	auto	it = findNamed(image);
	if (it != _namedImages.end()) {
		*it = _namedImages.back();
		_namedImages.pop_back();
	}
}

void	ImagePool::release(Image *image) {
	for (auto &pool: _pools) {
		for (auto &slot: pool.second) {
			if (slot.image.get() == image) {
				slot.inUse = false;
				removeIfNamed(image);
				return ;
			}
		}
	}
}

void	ImagePool::releaseAll(void) {
	for (auto &pool: _pools) {
		for (auto &slot: pool.second) {
			slot.inUse = false;
		}
	}
	_namedImages.clear();
}

}
