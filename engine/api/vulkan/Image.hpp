/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 15:33:47 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 19:38:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "utils/Ref.hpp"
#include "HelExpected.hpp"
#include "api/vulkan/ImageConfig.hpp"

namespace	hel {

class	Device;

class	Image {
	public:
		Image(const Image &) = delete;
		Image	operator=(const Image &) = delete;

		template <ImageType T>
		static expected<Ref<Image>>	create(Device *device,
										const ImageConfig<T> &config);
		static expected<Ref<Image>>	wrapImage(Device *device, VkImage image,
										VkFormat format, VkExtent2D extent);

		void	transitionLayout(VkCommandBuffer commandBuffer,
								VkImageLayout newLayout);

	private:
		Image(void) = default;
		~Image(void);

		template <ImageType T>
		expected<void>	init(Device *device, const ImageConfig<T> &config);
		expected<void>	init(Device *device, VkImage image,
							VkFormat format, VkExtent2D extent);

		expected<void>	validateConfig(void);
		void			deallocateImage(void);
		expected<void>	allocateImage(void);

		Device			*_device;
		ImageInfo		_config;

		VmaAllocation	_allocation{VK_NULL_HANDLE};
		VkImage			_image{VK_NULL_HANDLE};
};

}

#include "api/vulkan/Image.tpp"
