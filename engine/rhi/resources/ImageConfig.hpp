/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImageConfig.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 18:05:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/28 15:17:59                                        */
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
#include <algorithm>
#include <string>

#include "utils/Setters.hpp"
#include "utils/mathUtils.hpp"
#include "rhi/generated/HelFormatUtils.hpp"
#include "utils/typeHelper.hpp"

namespace	hel {

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

struct	ImageInfo {
	protected:
		// ImageInfo cannot be constructed directly.
		// Use ImageConfig1D / ImageConfig2D / ImageConfig3D / ImageConfigCube instead.
		ImageInfo(VkImageType type, const std::string &imageName)
			:	_type(type),
				_imageName(imageName) {}

		// Default constructor here to allow ImageInfo to be a class member
		ImageInfo(void) {}

		VkImageUsageFlags			_usage{0};
		VmaAllocationCreateFlags	_allocFlags{0};
		std::vector<VkFormat>		_formats{};
		VkExtent3D					_extent{1u, 1u, 1u};
		uint32_t					_layers{1u};

		VkImageType					_type;
		VkImageAspectFlags			_aspect;

		std::string					_imageName;
		bool						_owning{true};


	public:
		bool	operator==(const ImageInfo &other) const {
			return (this->_type == other._type &&
					this->_layers == other._layers &&
					this->_formats == other._formats &&
					this->_usage == other._usage &&
					this->_extent.width == other._extent.width &&
					this->_extent.height == other._extent.height &&
					this->_extent.depth == other._extent.depth);
		}

	friend class	Image;
	friend class	ImagePool;
	template <ImageType T>
	friend struct	ImageConfig;
	friend struct	ImageInfoHasher;
};

struct	ImageInfoHasher {
	size_t	operator()(const ImageInfo &info) const {
		size_t	seed = 0;
		for (auto format: info._formats)
			hel::mathUtils::hashCombine(seed, format);
		hel::mathUtils::hashCombine(seed, info._layers, info._extent.width,
			info._extent.height, info._extent.depth, info._usage, info._type);
		return (seed);
	}
};

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

		template <size_t N>
		static constexpr bool sameAspect(const std::array<VkFormat, N> &formats) {
			auto	a = getFormatAspect(formats[0]);
			for (auto f: formats) if (getFormatAspect(f) != a) return false;
			return true;
		}

		template <size_t N>
		static constexpr bool sameTexelSize(const std::array<VkFormat, N> &formats) {
			auto	s = getFormatTexelSize(formats[0]);
			for (auto f: formats) if (getFormatTexelSize(f) != s) return false;
			return true;
		}

		template <size_t N>
		static constexpr bool noDuplicates(const std::array<VkFormat, N> &formats) {
			for (size_t i = 0; i < N; ++i)
				for (size_t j = i + 1; j < N; ++j)
					if (formats[i] == formats[j]) return false;
			return true;
		}

		bool	formatsEqual(const std::vector<VkFormat> &other) {
			for (auto fmt: _formats) {
				if (std::find(other.begin(), other.end(), fmt) == other.end())
					return false;
			}
			return true;
		}

	public:
		ImageConfig(const std::string &imageName = "Unname image") : ImageInfo(T::imageType, imageName) {
			if constexpr (std::is_same_v<T, ImageTypeCube>) {
				_layers = 6;
				_usage |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			}
		}

		template <VkImageUsageFlags Usage>
		ImageConfig	&usage(void) {
			static_assert(Usage != 0,
				"Expecting a non-empty usage mask.");
			static_assert(Usage != VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM,
				"Invalid usage mask.");
			_usage |= Usage;
			return *this;
		}

		template <VkFormat First, VkFormat... Rest>
		ImageConfig	&formats(void) {
			constexpr std::array	arr{First, Rest...};

			static_assert(sameAspect(arr),
				"All formats must share the same aspect "
				"(color/depth/stencil).");
			static_assert(sameTexelSize(arr),
				"All formats must have the same byte size.");
			static_assert(noDuplicates(arr),
				"Duplicate format in the format list.");

			constexpr auto sorted_arr = [&arr]() {
				auto a = arr;
				for (size_t i = 0; i < a.size(); ++i) {
					for (size_t j = i + 1; j < a.size(); ++j) {
						if (static_cast<int>(a[i]) > static_cast<int>(a[j])) {
							std::swap(a[i], a[j]);
						}
					}
				}
				return a;
			}();

			_aspect = getFormatAspect(First);
			_formats.assign(sorted_arr.begin(), sorted_arr.end());
			return *this;
		}

		SETTER_VERBOSE_REQ(layers, uint32_t, T::has_layers)
		Extent	extent(void) {
			return {*this};
		}
		auto	extent2D(ToU32_v auto width) {
			_extent.width = max<uint32_t>(width, 1);
			return *this;
		}
		auto	extent2D(ToU32_v auto width, ToU32_v auto height) {
			_extent.width = max<uint32_t>(width, 1);
			_extent.height = max<uint32_t>(height, 1);
			return *this;
		}
		auto	extent3D(ToU32_v auto width, ToU32_v auto height, ToU32_v auto depth) {
			_extent.width = max<uint32_t>(width, 1);
			_extent.height = max<uint32_t>(height, 1);
			_extent.depth = max<uint32_t>(depth, 1);
			return *this;
		}
};

using ImageConfig1D		= ImageConfig<ImageType1D>;
using ImageConfig2D		= ImageConfig<ImageType2D>;
using ImageConfig3D		= ImageConfig<ImageType3D>;
using ImageConfigCube	= ImageConfig<ImageTypeCube>;

}
