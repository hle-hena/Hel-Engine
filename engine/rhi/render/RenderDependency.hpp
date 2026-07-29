/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderDependency.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/29 14:09:19 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/29 17:27:19                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "rhi/resources/ImageConfig.hpp"

#include <vector>
#include <optional>

namespace	hel {

struct	ImageAcces {
	public:
		ImageAcces(const ImageInfo &info);

	private:
		ImageInfo			_infos;
		VkFormat			_accessFormat;

	friend struct	RenderDependency;
};

struct	RenderDependency {
	public:
		template <ImageType T>
		RenderDependency	&addColorAttachment(const ImageConfig<T> &imageInfo);
		template <ImageType T>
		RenderDependency	&setDepthStencilAttachment(const ImageConfig<T> &imageInfo);
		template <ImageType T>
		RenderDependency	&setStencilAttachment(const ImageConfig<T> &imageInfo);
		template <ImageType T>
		RenderDependency	&setDepthAttachment(const ImageConfig<T> &imageInfo);

		template <ImageType T>
		RenderDependency	&addShaderRead(const ImageConfig<T> &imageInfo);
		template <ImageType T>
		RenderDependency	&addShaderWrite(const ImageConfig<T> &imageInfo);

	private:
		bool	alreadyContained(const ImageInfo &imageInfo);

		std::vector<ImageAcces>		_colorAttachments;
		std::optional<ImageAcces>	_depthAttachment;
		std::optional<ImageAcces>	_stencilAttachment;

		std::vector<ImageAcces>		_shaderReads;
		std::vector<ImageAcces>		_shaderWrites;
};

}
