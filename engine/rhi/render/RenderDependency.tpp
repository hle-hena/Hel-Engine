/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderDependency.tpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/29 16:41:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/29 17:49:03                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/render/RenderDependency.hpp"
#include "utils/Logger.hpp"

namespace	hel {

using RDep = RenderDependency;

template <ImageType T>
RDep	&RDep::addColorAttachment(const ImageConfig<T> &imageInfo) {
	if (alreadyContained(imageInfo))
		return *this;

	if (!(imageInfo._aspect & VK_IMAGE_ASPECT_COLOR_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as color attachment, "
			"however the image's format (\"{}\") does not support it.",
			imageInfo._imageName, getFormatName(imageInfo._formats[0]));
		return *this;
	}

	_colorAttachments.push_back(imageInfo);
	return *this;
}

template <ImageType T>
RDep	&RDep::setDepthStencilAttachment(const ImageConfig<T> &imageInfo) {
	if (alreadyContained(imageInfo))
		return *this;

	if (_depthAttachment.has_value() || _stencilAttachment.has_value()) {
		std::string_view	depAlreadySet;
		std::string			setByImage;
		if (_depthAttachment.has_value() && _stencilAttachment.has_value()) {
			depAlreadySet = "depth and stencil attachments were";
			setByImage = _depthAttachment->_infos._imageName + " and "
				+ _stencilAttachment->_infos._imageName + " respectively";
		}
		else if (_depthAttachment.has_value()) {
			depAlreadySet = "depth attachment was";
			setByImage = _depthAttachment->_infos._imageName;
		}
		else {
			depAlreadySet = "stencil attachment was";
			setByImage = _stencilAttachment->_infos._imageName;
		}

		HEL_ERROR("Trying to set depth stencil attachment with image \"{}\" "
			"while the {} already set by \"{}\".", imageInfo._imageName,
			depAlreadySet, setByImage);
		return *this;
	}

	if (imageInfo._aspect != (VK_IMAGE_ASPECT_DEPTH_BIT
								| VK_IMAGE_ASPECT_STENCIL_BIT))
	{
		HEL_ERROR("Trying to set image \"{}\" as depth stencil attachment, "
			"however the image's format (\"{}\") does not support it.",
			imageInfo._imageName, getFormatName(imageInfo._formats[0]));
		return *this;
	}
	_depthAttachment = {imageInfo};
	_stencilAttachment = {imageInfo};
	return *this;
}

template <ImageType T>
RDep	&RDep::setDepthAttachment(const ImageConfig<T> &imageInfo) {
	if (alreadyContained(imageInfo))
		return *this;

	if (_depthAttachment.has_value()) {
		HEL_ERROR("Trying to set depth attachment with image \"{}\" while the "
			"depth was already set by \"{}\".", imageInfo._imageName,
			_depthAttachment->_infos._imageName);
		return *this;
	}

	if (!(imageInfo._aspect & VK_IMAGE_ASPECT_DEPTH_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as depth attachment, "
			"however the image's format (\"{}\") does not support it.",
			imageInfo._imageName, getFormatName(imageInfo._formats[0]));
		return *this;
	}
	_depthAttachment = {imageInfo};
	return *this;
}

template <ImageType T>
RDep	&RDep::setStencilAttachment(const ImageConfig<T> &imageInfo) {
	if (alreadyContained(imageInfo))
		return *this;

	if (_stencilAttachment.has_value()) {
		HEL_ERROR("Trying to set stencil attachment with image \"{}\" while the "
			"stencil was already set by \"{}\".", imageInfo._imageName,
			_stencilAttachment->_infos._imageName);
		return *this;
	}

	if (!(imageInfo._aspect & VK_IMAGE_ASPECT_STENCIL_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as stencil attachment, "
			"however the image's format (\"{}\") does not support it.",
			imageInfo._imageName, getFormatName(imageInfo._formats[0]));
		return *this;
	}
	_stencilAttachment = {imageInfo};
	return *this;
}

template <ImageType T>
RDep	&RDep::addShaderRead(const ImageConfig<T> &imageInfo) {
	if (alreadyContained(imageInfo))
		return *this;

	if (!(imageInfo._usage & VK_IMAGE_USAGE_SAMPLED_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as shader read, however the "
			"image's usage does not support it. The image needs the usage "
			"VK_IMAGE_USAGE_SAMPLED_BIT.", imageInfo._imageName,
			getFormatName(imageInfo._formats[0]));
		return *this;
	}
	return *this;
}

template <ImageType T>
RDep	&RDep::addShaderWrite(const ImageConfig<T> &imageInfo) {
	if (alreadyContained(imageInfo))
		return *this;

	if (!(imageInfo._usage & VK_IMAGE_USAGE_STORAGE_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as shader write, however the "
			"image's usage does not support it. The image needs the usage "
			"VK_IMAGE_USAGE_STORAGE_BIT.", imageInfo._imageName,
			getFormatName(imageInfo._formats[0]));
		return *this;
	}
	return *this;
}

}
