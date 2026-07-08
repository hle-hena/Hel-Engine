/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PhaseDependency.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 15:14:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 18:21:47                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "utils/Setters.hpp"
#include "api/vulkan/Image.hpp"

#include <vulkan/vulkan.h>
#include <vector>
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
	};


	SETTER_VERBOSE(config, Image::Config)
	SETTER_VERBOSE(load, VkAttachmentLoadOp)
	SETTER_VERBOSE(store, VkAttachmentStoreOp)
	SETTER_VERBOSE(clearValue, VkClearValue)

	PhaseDependencies	*addDep(void);

	bool operator==(const ImageDep&) const;

	private:
		ImageDep(PhaseDependencies *parent, const std::string &imgName,
				VkFormat fmt, Usage usage, uint32_t bindingIndex)
			:	_parent(parent), _imageName(imgName), _format(fmt),
				_usage(usage), _bindingIndex(bindingIndex)	{}
		ImageDep(PhaseDependencies *parent, const std::string &imgName,
				VkFormat fmt, Usage usage)
			:	_parent(parent), _imageName(imgName), _format(fmt),
				_usage(usage)	{}

		PhaseDependencies					*_parent;

		std::string							_imageName;
		VkFormat							_format;
		Usage								_usage;
		std::optional<Image::Config>		_config;
		std::optional<VkClearValue>			_clearValue;
		VkAttachmentLoadOp					_load{VK_ATTACHMENT_LOAD_OP_MAX_ENUM};
		VkAttachmentStoreOp					_store{VK_ATTACHMENT_STORE_OP_MAX_ENUM};
		uint32_t							_bindingIndex;

	friend struct PhaseDependencies;
	friend struct DepHasher;
	friend class RenderPass;
	friend struct SystemManager;
	friend struct LayerState;
};

struct	PhaseDependencies {
	std::vector<std::string_view>	require{};
	std::vector<std::string_view>	block{};
	std::vector<std::string_view>	layers{};

	std::vector<ImageDep>			write;
	std::vector<std::string_view>	read;

	auto	*addBlock(std::initializer_list<std::string_view> list) {
		for (auto &elem: list)
			block.push_back(elem);
		return this;
	}
	auto	*addBlock(std::string_view newOne) {
		block.push_back(newOne);
		return this;
	}
	auto	*addRequire(std::initializer_list<std::string_view> list) {
		for (auto &elem: list)
			require.push_back(elem);
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
		return ImageDep(this, imageName, format, U);
	}
	template <ImageDep::Usage U>
	requires (U == ImageDep::Color)
	auto	startWrite(const std::string &imageName, VkFormat format,
					uint32_t bindingIndex) {
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
