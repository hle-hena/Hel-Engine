/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderDependency.tpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/29 16:41:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/30 11:23:04                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/render/RenderDependency.hpp"

namespace	hel {

using RDep = RenderDependency;

template <ImageAccessType T>
RDep	&RDep::addDep(const ImageAccess_T<T> &image) {
	if (alreadyContained(image))
		return *this;

	if constexpr (std::is_same_v<T, ImageAccessColor>) {
		addColorAttachment(image);
	} else if constexpr (std::is_same_v<T, ImageAccessDepthStencil>) {
		setDepthAttachment(image);
		setStencilAttachment(image);
	} else if constexpr (std::is_same_v<T, ImageAccessDepth>) {
		setDepthAttachment(image);
	} else if constexpr (std::is_same_v<T, ImageAccessStencil>) {
		setStencilAttachment(image);
	} else if constexpr (std::is_same_v<T, ImageAccessShaderRead>) {
		addShaderRead(image);
	} else if constexpr (std::is_same_v<T, ImageAccessShaderWrite>) {
		addShaderWrite(image);
	}
}

}
