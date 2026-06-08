/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PhaseDependancy.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/05 12:15:03 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/08 15:47:43                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "utils/Setters.hpp"
#include "api/vulkan/Image.hpp"

#include <string>
#include <optional>

namespace	hel {

struct	ImageDep {
	enum	Usage {
		Color = 1,
		Depth = 2,
		Stencil = 4,
		DepthStencil = Depth | Stencil,
		MAX_ENUM
	};

	SETTER(ImageName, std::string, imageName)
	SETTER(ImageUsage, Usage, usage)
	SETTER(ImageConfig, Image::Config, config)
	SETTER(FormatAsked, VkFormat, format)
	SETTER(LoadOp, VkAttachmentLoadOp, load)
	SETTER(StoreOp, VkAttachmentStoreOp, store)
	SETTER(ClearValue, VkClearValue, clear)
	SETTER(WriteBindingIndex, int, bindingIndex)

	std::string							imageName;
	VkFormat							format{VK_FORMAT_MAX_ENUM};
	Usage								usage{MAX_ENUM};
	std::optional<Image::Config>		config;
	std::optional<VkClearValue>			clear;
	VkAttachmentLoadOp					load{VK_ATTACHMENT_LOAD_OP_MAX_ENUM};
	VkAttachmentStoreOp					store{VK_ATTACHMENT_STORE_OP_MAX_ENUM};
	std::optional<int>					bindingIndex;

	bool operator==(const ImageDep&) const;
};

struct	PhaseDependencies {
	std::vector<std::string>	require{};
	std::vector<std::string>	block{};
	std::optional<std::string>	provides;

	std::vector<ImageDep>		write;
	std::vector<ImageDep>		read;

	bool operator==(const PhaseDependencies&) const;
};

struct	DepHasher {
	size_t	operator()(const PhaseDependencies &dep) const;
};

}
