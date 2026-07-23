/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImageViewConfig.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/20 18:33:04 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/23 12:09:11                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>

#include "utils/Setters.hpp"
#include "utils/mathUtils.hpp"

namespace	hel {

struct	ViewConfig {
	private:
		struct	ComponentMapping {
			ComponentMapping(ViewConfig &parentStruct)
				:	_parent(parentStruct)	{}

			auto	&identity(void) {
				_parent._components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				_parent._components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				_parent._components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				_parent._components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				return _parent;
			}
			auto	&RRR1(void) {
				_parent._components.r = VK_COMPONENT_SWIZZLE_R;
				_parent._components.g = VK_COMPONENT_SWIZZLE_R;
				_parent._components.b = VK_COMPONENT_SWIZZLE_R;
				_parent._components.a = VK_COMPONENT_SWIZZLE_ONE;
				return _parent;
			}

			PROXY_SETTER(r, VkComponentSwizzle, _parent._components.r)
			PROXY_SETTER(g, VkComponentSwizzle, _parent._components.g)
			PROXY_SETTER(b, VkComponentSwizzle, _parent._components.b)
			PROXY_SETTER(a, VkComponentSwizzle, _parent._components.a)

			private:
				ViewConfig	&_parent;
		};

		VkFormat			_format{VK_FORMAT_MAX_ENUM};
		VkImageAspectFlags	_aspect{VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM};
		VkComponentMapping	_components{
								.r = VK_COMPONENT_SWIZZLE_MAX_ENUM,
								.g = VK_COMPONENT_SWIZZLE_MAX_ENUM,
								.b = VK_COMPONENT_SWIZZLE_MAX_ENUM,
								.a = VK_COMPONENT_SWIZZLE_MAX_ENUM};

	public:
		bool	operator==(const ViewConfig &o) const {
			return (this->_format == o._format
				&& this->_components.r == o._components.r
				&& this->_components.g == o._components.g
				&& this->_components.b == o._components.b
				&& this->_components.a == o._components.a);
		}

		ComponentMapping	components(void)	{ return {*this}; }
		SETTER_VERBOSE(format, VkFormat)
		SETTER_VERBOSE(aspect, VkImageAspectFlags)

		auto	&defaultTextureView(void) {
			_format = VK_FORMAT_R8G8B8A8_SRGB;
			ComponentMapping(*this).identity();
			return *this;
		}

	friend struct	ViewConfigHasher;
	friend class	Image;
};
struct	ViewConfigHasher {
	size_t	operator()(const ViewConfig &conf) const {
		size_t	hash = 0;
		mathUtils::hashCombine(hash, conf._format, conf._components.r,
			conf._components.g, conf._components.b, conf._components.a);
		return hash;
	}
};

}
