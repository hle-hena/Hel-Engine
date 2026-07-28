/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderRequest.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/25 17:58:42 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/28 19:51:35                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string>
#include <any>
#include <unordered_map>
#include <glm/glm.hpp>

#include "rhi/resources/Image.hpp"
#include "utils/str_utils.hpp"

namespace	hel {

struct	RenderRequest {
	public:
		struct	Builder;

		template <typename T>
		const T	*tag(void) const {
			auto	retTag = std::any_cast<const T>(&_tag);
			if (!retTag) {
				HEL_FATAL("Failed to retrieve the variable from the render "
					"request. Tag's type ({}) doesn't match.",
					getTypeName(_tag.type().name()));
				return nullptr;
			}
			return retTag;
		}
		Ref<Image>			image(const std::string &imageName) const {
			auto	it = _images.find(imageName);
			if (it == _images.end()) {
				HEL_FATAL("The image \"{}\" doesn't exist in the render "
					"request.", imageName);
				return nullptr;
			}
			return it->second;
		}

		const VkExtent2D	*origin(void) const			{ return &_origin; }
		const VkExtent2D	*extent(void) const			{ return &_extent; }
		const std::string	*requestType(void) const	{ return &_requestType; }

		const VkExtent2D	&origin_v(void) const		{ return _origin; }
		const VkExtent2D	&extent_v(void) const		{ return _extent; }
		const std::string	&requestType_v(void) const	{ return _requestType; }

		bool	operator==(const RenderRequest &other) const;

	private:
		RenderRequest(void) = default;

		std::string									_requestType;
		std::any									_tag;
		VkExtent2D									_origin{0, 0};
		VkExtent2D									_extent{1, 1};
		std::unordered_map<std::string, Ref<Image>>	_images{};

	friend struct	std::hash<RenderRequest>;
	friend class	RenderPass;
};

struct	RenderRequest::Builder {
	private:
		RenderRequest	_request;

	public:
		Builder(const std::string requestType, ToU32_v auto width,
				ToU32_v auto height)
		{
			_request._requestType = requestType;
			_request._extent = {max<uint32_t>(width, 1),
								max<uint32_t>(height, 1)};
		}

		[[nodiscard]] RenderRequest build() && {
			return std::move(_request);
		}

		Builder	&&origin(ToU32_v auto width, ToU32_v auto height) && {
			_request._origin.width = ToU32(width);
			_request._origin.height = ToU32(height);
			return std::move(*this);
		}
		Builder	&&tag(std::any tag) && {
			_request._tag = std::move(tag);
			return std::move(*this);
		}
		Builder	&&addImage(const std::string &imageName, Ref<Image> image) && {
			if (_request._images.contains(imageName))
				HEL_ERROR("The image {} was already added in the image list.",
					imageName);
			else
				_request._images[imageName] = image;
			return std::move(*this);
		}

		Builder	&origin(ToU32_v auto width, ToU32_v auto height) & {
			_request._origin.width = ToU32(width);
			_request._origin.height = ToU32(height);
			return *this;
		}
		Builder	&tag(std::any tag) & {
			_request._tag = std::move(tag);
			return *this;
		}
		Builder	&addImage(const std::string &imageName, Ref<Image> image) & {
			if (_request._images.contains(imageName))
				HEL_ERROR("The image {} was already added in the image list.",
					imageName);
			else
				_request._images[imageName] = image;
			return *this;
		}

		bool	containsImageName(const std::string &imageName) const {
			return _request._images.contains(imageName);
		}
};

}

namespace std {
	template <>
	struct hash<hel::RenderRequest> {
		size_t operator()(const hel::RenderRequest &r) const;
	};
}
