/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/11 10:59:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/11 15:32:15                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <unordered_map>
# include <memory>

# include "api/vulkan/Image.hpp"

namespace	hel {

class	Device;

class	ImagePool {
	public:
		template <typename T>
		using ImageDescMap = std::unordered_map<Image::Config, T,
												Image::ConfigHasher>;

		class	Builder {
			public:
				Builder(Device &device);

				Builder	&addImage(const Image::Config &config, uint32_t count);

				std::unique_ptr<ImagePool>	build(void);

			private:
				Device					&_device;
				ImageDescMap<uint32_t>	_imageDescs;
		};

		Image	*acquire(const Image::Config &config);
		void	release(Image *);

		~ImagePool(void);

	private:
		struct	Slot {
			std::unique_ptr<Image>	image;
			bool					inUse{false};
		};

		ImagePool(Device &device, ImageDescMap<uint32_t> &&imageDescs);

		Device							&_device;
		ImageDescMap<std::vector<Slot>>	_pools;
};

}
