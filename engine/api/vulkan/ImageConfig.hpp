/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImageConfig.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 18:05:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 18:34:14                                        */
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
#include <vector>
#include <cstdint>

#include "utils/Setters.hpp"

namespace	hel {

struct	ImageInfo {
	protected:
		VkImageUsageFlags			_usage;
		VmaAllocationCreateFlags	_allocFlags;
		std::vector<VkFormat>		_formats;
		VkExtent3D					_extent{1u, 1u, 1u};
		uint32_t					_layers{1u};
		VkImageType					_type;

	friend class	Image;
};

struct	ImageType1D {
	static constexpr VkImageType	imageType = VK_IMAGE_TYPE_1D;
	static constexpr bool	has_layers = true;
	static constexpr bool	has_height = false;
	static constexpr bool	has_depth = false;
};

struct	ImageType2D {
	static constexpr VkImageType	imageType = VK_IMAGE_TYPE_2D;
	static constexpr bool	has_layers = true;
	static constexpr bool	has_height = true;
	static constexpr bool	has_depth = false;
};

struct	ImageType3D {
	static constexpr VkImageType	imageType = VK_IMAGE_TYPE_3D;
	static constexpr bool	has_layers = false;
	static constexpr bool	has_height = true;
	static constexpr bool	has_depth = true;
};

struct	ImageTypeCube {
	static constexpr VkImageType	imageType = VK_IMAGE_TYPE_2D;
	static constexpr bool	has_layers = false;
	static constexpr bool	has_height = true;
	static constexpr bool	has_depth = false;
};

template <typename T>
concept	ImageType =
	std::is_same_v<T, ImageType1D> ||
	std::is_same_v<T, ImageType2D> ||
	std::is_same_v<T, ImageType3D> ||
	std::is_same_v<T, ImageTypeCube>;

template <ImageType T>
struct	ImageConfig: public ImageInfo {
	private:
		struct	Extent {
			Extent(ImageConfig<T> &parent):	_parent(parent){}

			PROXY_SETTER(width, uint32_t, _parent._extent.width)
			PROXY_SETTER_REQ(height, uint32_t, _parent._extent.height, T::has_height)
			PROXY_SETTER_REQ(depth, uint32_t, _parent._extent.depth, T::has_depth)

			private:
				ImageConfig<T>	&_parent;
		};

	public:
		SETTER_VERBOSE(usage, VkImageUsageFlags)
		SETTER_VERBOSE(allocFlags, VmaAllocationCreateFlags)
		SETTER_VERBOSE(formats, std::initializer_list<VkFormat>)
		SETTER_VERBOSE_REQ(layers, uint32_t, T::has_layers)
		Extent	extent(void) {
			return {*this};
		}
};

using ImageConfig1D		= ImageConfig<ImageType1D>;
using ImageConfig2D		= ImageConfig<ImageType2D>;
using ImageConfig3D		= ImageConfig<ImageType3D>;
using ImageConfigCube	= ImageConfig<ImageTypeCube>;

}
