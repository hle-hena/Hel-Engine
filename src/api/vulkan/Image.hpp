/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/25 13:16:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/25 17:22:58                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <memory>
# include <vulkan/vulkan.h>

namespace	hel {

class	Device;

class Image {
	public:
		struct	Config;
		static std::unique_ptr<Image>	create(Device &device,
											const Config &config);
		~Image(void);

		Image(const Image &) = delete;
		Image	operator=(const Image &) = delete;

		void	transitionLayout(VkCommandBuffer cmd, VkImageLayout oldLayout, VkImageLayout newLayout);

		VkImage			getImage() const { return (_image); }
		VkImageView		getView() const { return (_view); }

	private:
		Image(Device &device, const Config &config);

		void	createImage(const Config &config);
		void	allocateMemory(const Config &config);
		void	createView(const Config &config);

		Device			&_device;
		VkImage			_image{VK_NULL_HANDLE};
		VkImageView		_view{VK_NULL_HANDLE};
		VkDeviceMemory	_memory{VK_NULL_HANDLE};
};

struct	Image::Config {
	uint32_t				width, height;
	VkFormat				format;
	VkImageUsageFlags		usage;
	VkMemoryPropertyFlags	properties;
	VkImageAspectFlags		aspectFlags;
};

}
