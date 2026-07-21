/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/11 10:59:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/21 17:46:59                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "api/vulkan/Image.hpp"

namespace	hel {

class	Device;

class	ImagePool {
	public:

		static Ref<ImagePool>	create(Device *device);

		template <ImageType T>
		Ref<Image>	acquire(const ImageConfig<T> &requested);
		// Image	*acquire(const Image::Config &requested, uint32_t life = 1u);
		// Image	*acquire(const std::string &referenceID,
		// 				const Image::Config &requested, uint32_t life = 1u);
		// Image	*get(const std::string &referenceID);
		// void	release(Image *);
		// void	releaseAll(void);

		~ImagePool(void);

	private:
		struct	Slot {
			Ref<Image>	image{nullptr};
			uint32_t	life{0};
		};

		ImagePool(Device *device);

		expected<uint64_t>	candidateScore(const ImageInfo &requested,
						const ImageInfo &candidate);

		// bool		candidateFits(const Image::Config &requested,
		// 				const Image::Config &candidate);
		// uint64_t	candidateScore(const Image::Config &requested,
		// 				const Image::Config &candidate);

		template <typename T>
		using ImageInfoMap = std::unordered_map<ImageInfo, T, ImageInfoHasher>;
		Device							*_device;
		ImageInfoMap<std::vector<Slot>>	_pools;
};

}
