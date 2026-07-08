/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/11 10:59:47 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 15:31:04                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/ImagePool.hpp"

#include <algorithm>
#include <iostream>
#include <bit>

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
		for (uint32_t i = 0; i < it.second; i++) {
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
	bool	aspectFit = (requested.aspectFlags & candidate.aspectFlags)
													== requested.aspectFlags;
	bool	formatFit = true;
	for (auto fmt: requested.format) {
		if (std::find(candidate.format.begin(), candidate.format.end(), fmt)
													== candidate.format.end())
			formatFit = false;
	}

	return (sizeFit && usageFit && aspectFit && formatFit);
}

uint64_t	ImagePool::candidateScore(const Image::Config &requested,
									const Image::Config &candidate) {
	uint64_t	waste = 0;

	waste += (candidate.width - requested.width) *
			(candidate.height - requested.height) * 100;
	waste += static_cast<uint64_t>(std::popcount(candidate.usage & ~requested.usage));
	waste += static_cast<uint64_t>(std::popcount(candidate.aspectFlags & ~requested.aspectFlags));
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

Image	*ImagePool::acquire(const Image::Config &requested, uint32_t life) {
	Slot					*bestSlot = nullptr;
	uint64_t				bestScore = UINT64_MAX;
	const Image::Config		*bestConfig = nullptr;

	for (auto &[candidate, pool]: _pools) {
		if (!candidateFits(requested, candidate))
			continue ;
		uint64_t	score = candidateScore(requested, candidate);
		if (score >= bestScore)
			continue ;
		bestConfig = &candidate;
		for (auto &slot: pool) {
			if (slot.life)
				continue ;
			bestSlot = &slot;
			bestScore = score;
			break ;
		}
	}
	if (!bestConfig)
		bestConfig = &requested;
	if (!bestSlot) {
		Slot	newSlot = {};
		newSlot.image = Image::create(_device, *bestConfig);
		if (!newSlot.image)
			return (nullptr);
		bestSlot = &_pools[*bestConfig].emplace_back(std::move(newSlot));
	}
	bestSlot->life = life;
	bestSlot->image->setExtent({std::max(requested.width, 1u), std::max(requested.height, 1u)}, {});
	bestSlot->image->resetWrittenState({});
	return (bestSlot->image.get());
}

Image	*ImagePool::acquire(const std::string &referenceID,
							const Image::Config &requested, uint32_t life) {
	if (findNamed(referenceID) != _namedImages.end()) {
		std::cerr << "Name already taken" << std::endl;
		return (nullptr);
	}
	auto	image = acquire(requested, life);
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
	//TODO -> optimise this function.
	for (auto &pool: _pools) {
		for (auto &slot: pool.second) {
			if (slot.image.get() == image) {
				slot.life = 0;
				removeIfNamed(image);
				return ;
			}
		}
	}
}

void	ImagePool::releaseAll(void) {
	for (auto &pool: _pools) {
		for (auto &slot: pool.second)
			if (slot.life > 0)	{
				if (--slot.life == 0)
					removeIfNamed(slot.image.get());
			}
	}
}

}
