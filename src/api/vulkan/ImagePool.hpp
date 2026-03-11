/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/11 10:59:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/11 11:55:32                                        */
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

namespace	hel {

class	Device;

struct	ImageDesc {
	VkFormat			format;
	VkExtent2D			extent;
	VkImageUsageFlags	usage;

	bool	operator==(const ImageDesc &other) const;
};

struct	ImageDescHasher {
	size_t	operator()(const ImageDesc &desc) const;
};

class	ImagePool {
	public:
		using ImageDescMap = std::unordered_map<ImageDesc, uint32_t,
												ImageDescHasher>;

		class	Builder {
			public:
				Builder(Device &device);

				Builder						&addImage(VkFormat format,
													VkExtent2D extent,
													VkImageUsageFlags usage,
													uint32_t count);
				std::unique_ptr<ImagePool>	build(void);

			private:
				Device			&_device;
				ImageDescMap	_imageDescs;
		};

		~ImagePool(void);

	private:
		ImagePool(Device &device, ImageDescMap &&imageDescs);

		Device	&_device;
};

}
