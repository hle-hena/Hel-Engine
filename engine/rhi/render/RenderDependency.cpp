/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderDependency.cpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/29 17:27:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/30 14:37:54                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/render/RenderDependency.hpp"
#include "utils/Logger.hpp"
#include "utils/vec_utils.hpp"

namespace	hel {

#define ALREADY_DEFINED(definedAs)	\
do {																		\
	HEL_ERROR("Trying to define new image \"{}\" in the render dependency,"	\
		" however, it was already defined as a " #definedAs ".",			\
		image._info._imageName);												\
} while (0)

RenderDependency	RenderDependency::combineDependencies(
						const std::vector<RenderDependency> &deps)
{
	RenderDependency	res;

	for (auto &dep: deps) {
		for (auto &color: dep._colorAttachments) {
			auto	[alreadyExist, it] = contains(res._colorAttachments, color);
			if (!alreadyExist)
				res.addShaderRead(color);
		}
		if (!res._depthAttachment && dep._depthAttachment)
			res.setDepthAttachment(*dep._depthAttachment);
		if (!res._stencilAttachment && dep._stencilAttachment)
			res.setStencilAttachment(*dep._stencilAttachment);
		for (auto &read: dep._shaderReads) {
			auto	[alreadyExist, it] = contains(res._shaderReads, read);
			if (!alreadyExist)
				res.addShaderRead(read);
		}
		for (auto &write: dep._shaderWrites) {
			auto	[alreadyExist, it] = contains(res._shaderWrites, write);
			if (!alreadyExist)
				res.addShaderRead(write);
		}
	}

	return res;	
}

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
	if (contains_if(_colorAttachments, compImages).first) {
		ALREADY_DEFINED(color attachment);
		return true;
	} else if (contains_if(_shaderReads, compImages).first) {
		ALREADY_DEFINED(shader read);
		return true;
	} else if (contains_if(_shaderWrites, compImages).first) {
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
	auto	[found, foundImage] = contains_if(_colorAttachments,
									[&](const ImageAccess &other){
										return other._index == image._index;
									});
	if (found) {
		HEL_ERROR("Trying to add the image \"{}\" as color attachment but the "
			"index {} was already set by image \"{}\".", image._info._imageName,
			image._index, foundImage->_info._imageName);
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
