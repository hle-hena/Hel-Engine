/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderDependency.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/29 14:09:19 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/30 14:45:12                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "rhi/resources/ImageConfig.hpp"
#include "utils/Logger.hpp"

#include <vector>
#include <optional>

namespace	hel {

struct	ImageAccessColor {
	static constexpr bool	hasClearColor = true;
	static constexpr bool	hasClearDepthStencil = false;
	static constexpr bool	hasLoadStoreOp = true;
	static constexpr bool	hasFormat = true;
	static constexpr bool	isOrdered = true;
};

struct	ImageAccessDepth {
	static constexpr bool	hasClearColor = false;
	static constexpr bool	hasClearDepthStencil = true;
	static constexpr bool	hasLoadStoreOp = true;
	static constexpr bool	hasFormat = false;
	static constexpr bool	isOrdered = false;
};

struct	ImageAccessStencil {
	static constexpr bool	hasClearColor = false;
	static constexpr bool	hasClearDepthStencil = true;
	static constexpr bool	hasLoadStoreOp = true;
	static constexpr bool	hasFormat = false;
	static constexpr bool	isOrdered = false;
};

struct	ImageAccessDepthStencil {
	static constexpr bool	hasClearColor = false;
	static constexpr bool	hasClearDepthStencil = true;
	static constexpr bool	hasLoadStoreOp = true;
	static constexpr bool	hasFormat = false;
	static constexpr bool	isOrdered = false;
};

struct	ImageAccessShaderRead {
	static constexpr bool	hasClearColor = false;
	static constexpr bool	hasClearDepthStencil = false;
	static constexpr bool	hasLoadStoreOp = false;
	static constexpr bool	hasFormat = true;
	static constexpr bool	isOrdered = false;
	// Technically, I could take an order, the binding index,
	// and another one for the set index.
	// And I could also take a format, being the format the view would be in.
};

struct	ImageAccessShaderWrite {
	static constexpr bool	hasClearColor = false;
	static constexpr bool	hasClearDepthStencil = false;
	static constexpr bool	hasLoadStoreOp = false;
	static constexpr bool	hasFormat = false;
	static constexpr bool	isOrdered = false;
	// Technically, I could take an order, the binding index,
	// and another one for the set index.
};

template <typename T>
concept	ImageAccessType =
	std::is_same_v<T, ImageAccessColor> &&
	std::is_same_v<T, ImageAccessDepth> &&
	std::is_same_v<T, ImageAccessStencil> &&
	std::is_same_v<T, ImageAccessDepthStencil> &&
	std::is_same_v<T, ImageAccessShaderRead> &&
	std::is_same_v<T, ImageAccessShaderWrite>;

struct	ImageAccess {
	protected:
		ImageAccess(const ImageInfo &info) : _info(info) {}

		ImageInfo							_info;
		std::optional<uint32_t>				_index;
		std::optional<VkFormat>				_format;
		VkClearValue						_clear;
		std::optional<VkAttachmentLoadOp>	_load;
		std::optional<VkAttachmentStoreOp>	_store;

	friend struct	RenderDependency;
};

template <ImageAccessType T>
struct	ImageAccess_T: public ImageAccess {
	public:
		template <ImageType U>
		ImageAccess_T(const ImageConfig<U> &imageInfo) : ImageAccess(imageInfo) {
			if constexpr (std::is_same_v<T, ImageAccessColor>) {
				_clear = VkClearColorValue{.float32 = {0.f, 0.f, 0.f, 0.f}};
			} else if constexpr (std::is_same_v<T, ImageAccessDepth>
							|| std::is_same_v<T, ImageAccessStencil>
							|| std::is_same_v<T, ImageAccessDepthStencil>)
			{
				_format = imageInfo._formats[0];
				_clear = VkClearDepthStencilValue{.depth = 1.f, .stencil = 0};
			}
		}

		SETTER_REQ(Clear, VkClearColorValue, _clear,
			T::hasClearColor)
		SETTER_REQ(Clear, VkClearDepthStencilValue, _clear,
			T::hasClearDepthStencil)
		SETTER_REQ(Index, uint32_t, _index,
			T::isOrdered)

		ImageAccess_T	&setLoadOp(VkAttachmentLoadOp loadOp)
			requires (T::hasLoadStoreOp)
		{
			if (loadOp == VK_ATTACHMENT_LOAD_OP_MAX_ENUM) {
				HEL_ERROR("Trying to set the load op of image \"{}\" as an "
					"invalid value (VK_ATTACHMENT_LOAD_OP_MAX_ENUM).",
					_info.imageName());
				return *this;
			}
			if (loadOp == VK_ATTACHMENT_LOAD_OP_NONE_KHR) {
				HEL_ERROR("Trying to set the load op of image \"{}\" as an "
					"invalid value (VK_ATTACHMENT_LOAD_OP_NONE_KHR).",
					_info.imageName());
				return *this;
			}
			_load = loadOp;
			return *this;
		}
		ImageAccess_T	&setStoreOp(VkAttachmentStoreOp storeOp)
			requires (T::hasLoadStoreOp)
		{
			if (storeOp == VK_ATTACHMENT_STORE_OP_MAX_ENUM) {
				HEL_ERROR("Trying to set the store op of image \"{}\" as an "
					"invalid value (VK_ATTACHMENT_STORE_OP_MAX_ENUM).",
					_info.imageName());
				return *this;
			}
			if (storeOp == VK_ATTACHMENT_STORE_OP_NONE) {
				HEL_ERROR("Trying to set the store op of image \"{}\" as an "
					"invalid value (VK_ATTACHMENT_STORE_OP_NONE).",
					_info.imageName());
				return *this;
			}
			_store = storeOp;
			return *this;
		}

		ImageAccess_T	&setFormat(VkFormat format) requires (T::hasFormat)
		{
			if (!_info.hasFormat(format)) {
				HEL_ERROR("Trying to set image acces format {} for image \"{}\""
					" but the config doesn't contain that format.",
					getFormatName(format), _info.imageName());
				return *this;
			}
			_format = format;
			return *this;
		}

	friend struct	RenderDependency;
};

struct	RenderDependency {
	public:
		template <ImageAccessType T>
		RenderDependency	&addDep(const ImageAccess_T<T> &image);

		static RenderDependency
			combineDependencies(const std::vector<RenderDependency> &deps);

	private:
		bool	alreadyContained(const ImageAccess &image);

		void	addColorAttachment(const ImageAccess &image);
		void	setDepthAttachment(const ImageAccess &image);
		void	setStencilAttachment(const ImageAccess &image);

		void	addShaderRead(const ImageAccess &image);
		void	addShaderWrite(const ImageAccess &image);

		std::vector<ImageAccess>		_colorAttachments;
		std::optional<ImageAccess>		_depthAttachment;
		std::optional<ImageAccess>		_stencilAttachment;

		std::vector<ImageAccess>		_shaderReads;
		std::vector<ImageAccess>		_shaderWrites;

	friend struct	RenderPass;
};

}
