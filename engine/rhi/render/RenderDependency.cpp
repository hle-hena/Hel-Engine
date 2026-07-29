/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderDependency.cpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/29 17:27:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/29 17:42:14                                        */
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
		imageInfo._imageName);												\
} while (0)

bool	RenderDependency::alreadyContained(const ImageInfo &imageInfo) {
	if (_depthAttachment.has_value() &&
		_depthAttachment.value()._infos == imageInfo)
	{
		ALREADY_DEFINED(depth attachment);
		return true;
	}
	if (_stencilAttachment.has_value() &&
		_stencilAttachment.value()._infos == imageInfo)
	{
		ALREADY_DEFINED(stencil attachment);
		return true;
	}

	auto	compImages = [&imageInfo](const ImageAcces &acces) -> bool {
		return acces._infos == imageInfo;
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

}
