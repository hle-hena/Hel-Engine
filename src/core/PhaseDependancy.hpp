/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PhaseDependancy.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/05 12:15:03 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/12 12:50:39                                        */
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

	SETTER(ImageUsage, Usage, usage)
	SETTER(ImageConfig, Image::Config, config)
	SETTER(LoadOp, VkAttachmentLoadOp, load)
	SETTER(StoreOp, VkAttachmentStoreOp, store)
	SETTER(ClearValue, VkClearValue, clear)
	SETTER(WriteBindingIndex, int, bindingIndex)

	std::string							imageName;
	VkFormat							format;
	Usage								usage{MAX_ENUM};
	std::optional<Image::Config>		config;
	std::optional<VkClearValue>			clear;
	VkAttachmentLoadOp					load{VK_ATTACHMENT_LOAD_OP_MAX_ENUM};
	VkAttachmentStoreOp					store{VK_ATTACHMENT_STORE_OP_MAX_ENUM};
	std::optional<int>					bindingIndex;

	ImageDep(const std::string &imgName, VkFormat fmt)
		:	imageName(imgName), format(fmt)	{}
	bool operator==(const ImageDep&) const;
};

struct	PhaseDependencies {
	std::vector<std::string_view>	require{};
	std::vector<std::string_view>	block{};
	std::optional<std::string>	provides;

	std::vector<ImageDep>			write;
	std::vector<std::string_view>	read;

	auto	*addBlock(std::string_view newOne) {
		block.push_back(newOne);
		return this;
	}
	auto	*addRequire(std::string_view newOne) {
		require.push_back(newOne);
		return this;
	}

	bool operator==(const PhaseDependencies&) const;
};

struct	DepHasher {
	size_t	operator()(const PhaseDependencies &dep) const;
};

}
