/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderDependency.cpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/29 17:27:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/30 11:22:22                                        */
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

#define ALREADY_DEFINED(definedAs)	\
do {																		\
	HEL_ERROR("Trying to define new image \"{}\" in the render dependency,"	\
		" however, it was already defined as a " #definedAs ".",			\
		image._info._imageName);												\
} while (0)

bool	RenderDependency::alreadyContained(const ImageAccess &image) {
	if (_depthAttachment.has_value() &&
		_depthAttachment.value()._info == image._info)
	{
		ALREADY_DEFINED(depth attachment);
		return true;
	}
	if (_stencilAttachment.has_value() &&
		_stencilAttachment.value()._info == image._info)
	{
		ALREADY_DEFINED(stencil attachment);
		return true;
	}

	auto	compImages = [&image](const ImageAccess &acces) -> bool {
		return acces._info == image._info;
	};
	auto	existsIn = [&compImages](auto cont) -> bool {
		auto	it = std::find_if(cont.begin(), cont.end(), compImages);
		return it != cont.end();
	};
	if (existsIn(_colorAttachments)) {
		ALREADY_DEFINED(color attachment);
		return true;
	} else if (existsIn(_shaderReads)) {
		ALREADY_DEFINED(shader read);
		return true;
	} else if (existsIn(_shaderWrites)) {
		ALREADY_DEFINED(shader write);
		return true;
	}

	return false;
}

void	RenderDependency::addColorAttachment(const ImageAccess &image) {
	if (!image._index.has_value()) {
		HEL_ERROR("Trying to add the image \"{}\" as color attachment but the "
			"order index wasn't set.", image._info._imageName);
		return ;
	}
	if (!image._format.has_value()) {
		HEL_ERROR("Trying to add the image \"{}\" as color attachment but the "
			"format wasn't set.", image._info._imageName);
		return ;
	}
	if (!(image._info._usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as color attachment, however "
			"the image's usage does not allow it. The image needs the usage "
			"VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT.",
			image._info._imageName);
		return ;
	}
	if (image._info._aspect != VK_IMAGE_ASPECT_COLOR_BIT) {
		HEL_ERROR("Trying to add the image \"{}\" as color attachment but the "
			"format given to the image ({}) isn't made for color.",
			image._info._imageName, getFormatName(image._info._formats[0]));
		return ;
	}
	auto	foundIndex = std::find_if(_colorAttachments.begin(),
								_colorAttachments.end(),
								[&](const ImageAccess &other){
									return other._index == image._index;
								});
	if (foundIndex != _colorAttachments.end()) {
		HEL_ERROR("Trying to add the image \"{}\" as color attachment but the "
			"index {} was already set by image \"{}\".", image._info._imageName,
			image._index, foundIndex->_info._imageName);
		return ;
	}
	_colorAttachments.push_back(image);
}

void	RenderDependency::setDepthAttachment(const ImageAccess &image) {
	if (_depthAttachment.has_value()) {
		HEL_ERROR("Trying to set the image \"{}\" as depth attachment but the "
			"depth was already set by image \"{}\"", image._info._imageName,
			_depthAttachment->_info._imageName);
		return ;
	}
	if (!(image._info._usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as depth attachment, however "
			"the image's usage does not allow it. The image needs the usage "
			"VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT.",
			image._info._imageName);
		return ;
	}
	if (!(image._info._aspect & VK_IMAGE_ASPECT_DEPTH_BIT)) {
		HEL_ERROR("Trying to set the image \"{}\" as depth attachment but the "
			"format given to the image ({}) isn't made for depth.",
			image._info._imageName, getFormatName(image._info._formats[0]));
		return ;
	}
	_depthAttachment = image;
}

void	RenderDependency::setStencilAttachment(const ImageAccess &image) {
	if (_stencilAttachment.has_value()) {
		HEL_ERROR("Trying to set the image \"{}\" as stencil attachment but the"
			" stencil was already set by image \"{}\"", image._info._imageName,
			_stencilAttachment->_info._imageName);
		return ;
	}
	if (!(image._info._usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as stencil attachment, however "
			"the image's usage does not allow it. The image needs the usage "
			"VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT.",
			image._info._imageName);
		return ;
	}
	if (!(image._info._aspect & VK_IMAGE_ASPECT_STENCIL_BIT)) {
		HEL_ERROR("Trying to set the image \"{}\" as stencil attachment but the"
			" format given to the image ({}) isn't made for stencil.",
			image._info._imageName, getFormatName(image._info._formats[0]));
		return ;
	}
	_stencilAttachment = image;

}

void	RenderDependency::addShaderRead(const ImageAccess &image) {
	if (!(image._info._usage & VK_IMAGE_USAGE_SAMPLED_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as shader read, however the "
			"image's usage does not allow it. The image needs the usage "
			"VK_IMAGE_USAGE_SAMPLED_BIT.", image._info._imageName);
		return ;
	}
	_shaderReads.push_back(image);
}

void	RenderDependency::addShaderWrite(const ImageAccess &image) {
	if (!(image._info._usage & VK_IMAGE_USAGE_STORAGE_BIT)) {
		HEL_ERROR("Trying to set image \"{}\" as shader write, however the "
			"image's usage does not allow it. The image needs the usage "
			"VK_IMAGE_USAGE_STORAGE_BIT.", image._info._imageName);
		return ;
	}
	_shaderWrites.push_back(image);
}

}
