/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PhaseDependancy.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/05 12:15:03 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/18 11:07:51                                        */
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

struct	PhaseDependencies;

struct	ImageDep {
	enum	Usage {
		Color = 1,
		Depth = 2,
		Stencil = 4,
		DepthStencil = Depth | Stencil,
		MAX_ENUM
	};


	SETTER_VERBOSE(config, Image::Config)
	SETTER_VERBOSE(load, VkAttachmentLoadOp)
	SETTER_VERBOSE(store, VkAttachmentStoreOp)
	SETTER_VERBOSE(clearValue, VkClearValue)

	PhaseDependencies	*addDep(void);

	bool operator==(const ImageDep&) const;

	private:
		ImageDep(PhaseDependencies *parent, const std::string &imgName,
				VkFormat fmt, Usage usage, int bindingIndex = -1)
			:	_parent(parent), _imageName(imgName), _format(fmt),
				_usage(usage)
			{
				if (bindingIndex != -1)
					_bindingIndex = bindingIndex;
			}

		PhaseDependencies					*_parent;

		std::string							_imageName;
		VkFormat							_format;
		Usage								_usage{MAX_ENUM};
		std::optional<Image::Config>		_config;
		std::optional<VkClearValue>			_clearValue;
		VkAttachmentLoadOp					_load{VK_ATTACHMENT_LOAD_OP_MAX_ENUM};
		VkAttachmentStoreOp					_store{VK_ATTACHMENT_STORE_OP_MAX_ENUM};
		std::optional<int>					_bindingIndex;

	friend struct PhaseDependencies;
	friend struct DepHasher;
	friend class RenderPass;
};

struct	PhaseDependencies {
	std::vector<std::string_view>	require{};
	std::vector<std::string_view>	block{};
	std::vector<std::string_view>	layers{};

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
	auto	*addActiveLayer(std::initializer_list<std::string_view> list) {
		for (auto &elem: list)
			layers.push_back(elem);
		return this;
	}
	auto	*addActiveLayer(std::string_view layer) {
		layers.push_back(layer);
		return this;
	}
	template <ImageDep::Usage U>
	requires (U != ImageDep::Color)
	auto	startWrite(const std::string &imageName, VkFormat format) {
		return ImageDep(this, imageName, format, U, -1);
	}
	template <ImageDep::Usage U>
	requires (U == ImageDep::Color)
	auto	startWrite(const std::string &imageName, VkFormat format,
					int bindingIndex) {
		return ImageDep(this, imageName, format, U, bindingIndex);
	}
	auto	*addRead(const std::string_view &imageName) {
		read.push_back(imageName);
		return this;
	}

	bool operator==(const PhaseDependencies&) const;
};

struct	DepHasher {
	size_t	operator()(const PhaseDependencies &dep) const;
};

}
