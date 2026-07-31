/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RuntimeDependency.hpp                                               */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/31 10:25:37 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/31 11:54:59                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "rhi/resources/Image.hpp"

#include <vulkan/vulkan.h>
#include <vector>

namespace	hel {

struct	ResolvedAccess_Img {
	Ref<Image>					image;
	// Either fetched or built during the compile phase

	VkFormat					format;
	VkImageLayout				requestedLayout;
	VkRenderingAttachmentInfo	renderingInfo;
	// Assuming the load and store are defined during a compile phase.
};

struct	PassDependencies {
	std::vector<VkRenderingAttachmentInfo>	_colorAttachmentsInfo;
	// Assuming the load and store are defined during a compile phase.
	std::vector<ResolvedAccess_Img>			_colorAttachments;
	// Assumes ordered attachments.
	std::optional<ResolvedAccess_Img>		_depthAttachment;
	std::optional<ResolvedAccess_Img>		_stencilAttachment;

	std::vector<ResolvedAccess_Img>			_shaderReads;
	std::vector<ResolvedAccess_Img>			_shaderWrites;
};

}
