/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/11 10:59:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/23 11:52:10                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <unordered_map>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "api/vulkan/Image.hpp"
#include "api/vulkan/Swapchain.hpp"

namespace	hel {

class	Device;

class	ImagePool: public RefCounted {
	public:

		static Ref<ImagePool>	create(Device *device);

		Ref<Image>	acquire(uint32_t frameIndex,
						const ImageInfo &requested);
		void		collectFromFrame(uint32_t frameIndex);
		void		evict(void);

		~ImagePool(void);

	private:
		using clock = std::chrono::steady_clock;
		template <typename T>
		using ImageInfoMap = std::unordered_map<ImageInfo, T, ImageInfoHasher>;

		static constexpr std::chrono::milliseconds	evictionThreshold{10000};
		struct	UnusedEntry {
			Ref<Image>			image;
			clock::time_point	releaseAt{clock::now()};
		};
		struct	Slots {
			std::vector<UnusedEntry>					unusedImages;

			std::array<std::vector<Ref<Image>>,
				Swapchain::MAX_FRAMES_IN_FLIGHT>	usedImages;
		};
		struct	Slot {
			Ref<Image>	image{nullptr};
			uint32_t	life{0};
		};

		ImagePool(Device *device);

		expected<uint64_t>	candidateScore(const ImageInfo &requested,
						const ImageInfo &candidate);

		Device				*_device;
		ImageInfoMap<Slots>	_pools;
};

}
