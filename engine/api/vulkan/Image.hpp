/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/25 13:16:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/15 11:28:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <memory>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <vector>
#include <vma/vk_mem_alloc.h>

#include "utils/Setters.hpp"
#include "HelExpected.hpp"
#include "utils/Ref.hpp"

namespace	hel {

class	Device;
class	Buffer;

struct	ViewConfig {
	bool	operator==(const ViewConfig &other) const;

	private:
		struct	ComponentMapping {
			ComponentMapping(ViewConfig &parentStruct)
				:	_parent(parentStruct)	{}

			auto	&identity(void) {
				_parent._components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				_parent._components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				_parent._components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				_parent._components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				return _parent;
			}
			auto	&RRR1(void) {
				_parent._components.r = VK_COMPONENT_SWIZZLE_R;
				_parent._components.g = VK_COMPONENT_SWIZZLE_R;
				_parent._components.b = VK_COMPONENT_SWIZZLE_R;
				_parent._components.a = VK_COMPONENT_SWIZZLE_ONE;
				return _parent;
			}

			PROXY_SETTER(r, VkComponentSwizzle, _parent._components.r, _parent)
			PROXY_SETTER(g, VkComponentSwizzle, _parent._components.g, _parent)
			PROXY_SETTER(b, VkComponentSwizzle, _parent._components.b, _parent)
			PROXY_SETTER(a, VkComponentSwizzle, _parent._components.a, _parent)

			private:
				ViewConfig	&_parent;
		};

		VkFormat			_format{VK_FORMAT_MAX_ENUM};
		VkImageAspectFlags	_aspect{VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM};
		VkComponentMapping	_components{
								.r = VK_COMPONENT_SWIZZLE_MAX_ENUM,
								.g = VK_COMPONENT_SWIZZLE_MAX_ENUM,
								.b = VK_COMPONENT_SWIZZLE_MAX_ENUM,
								.a = VK_COMPONENT_SWIZZLE_MAX_ENUM};

	public:
		ComponentMapping	components(void)	{ return {*this}; }
		SETTER_VERBOSE(format, VkFormat)
		SETTER_VERBOSE(aspect, VkImageAspectFlags)

		auto	&defaultTextureView(void) {
			_format = VK_FORMAT_R8G8B8A8_SRGB;
			_aspect = VK_IMAGE_ASPECT_COLOR_BIT;
			ComponentMapping(*this).identity();
			return *this;
		}

	friend struct	ViewConfigHasher;
	friend class	Image;
};
struct	ViewConfigHasher {
	size_t	operator()(const ViewConfig &conf) const;
};

class Image {
	public:
		using ptr = std::unique_ptr<Image>;
		struct	Config {
			SETTER(Formats, std::initializer_list<VkFormat>, format)
			SETTER_INIT(Formats, VkFormat, format)
			SETTER(Width, uint32_t, width)
			SETTER(Height, uint32_t, height)
			SETTER_OR(Usage, VkImageUsageFlags, usage)
			SETTER_OR(Aspect, VkImageAspectFlags, aspectFlags)

			uint32_t				width{4096}, height{4096};
			std::vector<VkFormat>	format{};
			VkImageUsageFlags		usage{0};
			VkImageAspectFlags		aspectFlags{0};

			bool	operator==(const Config &other) const;
		};
		struct	ConfigHasher {
			size_t	operator()(const Config &desc) const;
		};
		static std::unique_ptr<Image>	create(Device &device,
											const Config &config);
		static std::unique_ptr<Image>	wrapSwapchainImages(Device &device,
											VkImage image, VkFormat format,
											VkExtent2D extent);
		~Image(void);

		Image(const Image &) = delete;
		Image	operator=(const Image &) = delete;

		void	transitionLayout(VkCommandBuffer commandBuffer,
								VkImageLayout newLayout);
		expected<void>	setData(void *data, uint32_t count);
		void			copyTo(VkCommandBuffer commandBuffer, Image *dst);
		void			copyTo(VkCommandBuffer commandBuffer, Ref<Buffer> dst,
					VkOffset3D startPos, VkExtent3D extent);

		PASSKEY(PoolKey, ImagePool)
		void	setExtent(const VkExtent2D &extent, PoolKey)
			{ _extent = extent; }

		PASSKEY(RenderPassKey, RenderPass)
		void	setWrittenState(RenderPassKey)	{ _written = true; }
		void	resetWrittenState(PoolKey)	{ _written = false; }
		bool	wasWritten(void) const	{ return _written; }

		VkImage						getImage(void) const
			{ return (_image); }
		VkExtent2D					getExtent(void) const
			{ return (_extent); }
		VkExtent2D					getPhysicalExtent(void) const
			{ return {_config.width, _config.height}; }
		VkFormat					getFormat(void) const
			{ return (_config.format[0]); }

		VkImageView					getView(const ViewConfig &conf);
		VkDescriptorSet				getTexture(VkImageView view);
		VkDescriptorImageInfo		getDescriptorInfo(VkImageView view) const;
		VkRenderingAttachmentInfo	getRenderingInfo(VkClearValue clearValue,
				VkAttachmentLoadOp loadOp,
				VkAttachmentStoreOp storeOp,
				VkImageView view) const;

	private:
		Image(Device &device, const Config &config);
		Image(Device &device, VkImage img, VkFormat format, VkExtent2D extent);

		void		createImage(void);
		VkImageView	createView(const ViewConfig &conf);

		void	setWrittenState(void)	{ _written = true; }

		bool							_owned{true};
		bool							_written{false};
		Device							&_device;
		Config							_config;
		VkExtent2D						_extent;
		VkImage							_image{VK_NULL_HANDLE};
		std::unordered_map<ViewConfig,
					VkImageView,
					ViewConfigHasher>	_views;
		std::unordered_map<VkImageView,
					VkDescriptorSet>	_textures;
		VmaAllocation					_allocation{VK_NULL_HANDLE};
		VkImageLayout					_currentLayout{VK_IMAGE_LAYOUT_UNDEFINED};
};

}
