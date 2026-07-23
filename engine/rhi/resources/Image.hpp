/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 15:33:47 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/23 14:45:38                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "utils/Ref.hpp"
#include "HelExpected.hpp"
#include "api/vulkan/ImageConfig.hpp"
#include "api/vulkan/ImageViewConfig.hpp"

namespace	hel {

class	Device;
class	Buffer;

class	Image: public RefCounted {
	public:
		Image(const Image &) = delete;
		Image	operator=(const Image &) = delete;

		static Ref<Image>	create(Device *device,
										const ImageInfo &config);
		template <VkFormat Format>
		static Ref<Image>	wrapImage(Device *device, VkImage image,
										VkExtent2D extent);

		void	transitionLayout(VkCommandBuffer commandBuffer,
								VkImageLayout newLayout);

		Ref<Buffer>			setData(VkCommandBuffer commandBuffer,
								const std::vector<unsigned char> &src);
		void			copyTo(VkCommandBuffer commandBuffer, Ref<Image> dst);
		void			copyTo(VkCommandBuffer commandBuffer, Ref<Buffer> dst,
								VkOffset3D startPos, VkExtent3D extent);

		PASSKEY(PoolKey, ImagePool)
		void	setExtent(const VkExtent3D &extent, PoolKey)
			{ _extent = extent; }

		PASSKEY(RenderPassKey, RenderPass)
		void	setWrittenState(RenderPassKey)	{ _written = true; }
		void	resetWrittenState(PoolKey)	{ _written = false; }
		bool	wasWritten(void) const	{ return _written; }

		VkImage						getImage(void) const
			{ return _image; }
		VkExtent3D					getExtent(void) const
			{ return _extent; }
		VkExtent2D					getExtent2D(void) const
			{ return {_extent.width, _extent.height}; }
		VkExtent3D					getPhysicalExtent(void) const
			{ return _config._extent; }

		VkImageView					getView(ViewConfig conf);
		VkDescriptorSet				getTexture(VkImageView view);
		VkDescriptorImageInfo		getDescriptorInfo(VkImageView view) const;
		VkRenderingAttachmentInfo	getRenderingInfo(VkClearValue clearValue,
				VkAttachmentLoadOp loadOp,
				VkAttachmentStoreOp storeOp,
				VkImageView view) const;

	private:
		Image(void) = default;
		~Image(void);

		expected<void>	init(Device *device, const ImageInfo &config);
		template <VkFormat Format>
		expected<void>	init(Device *device, VkImage image, VkExtent2D extent);

		expected<void>			validateConfig(void);
		void					deallocateImage(void);
		expected<void>			allocateImage(void);
		expected<VkImageView>	createView(const ViewConfig &conf);

		expected<void>	validateSetData(const std::vector<unsigned char> &src);
		expected<void>	validateCopy(Ref<Image> dst);

		void	setWrittenState(void)	{ _written = true; }

		Device				*_device;
		ImageInfo			_config;
		VkImageAspectFlags	_aspect{VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM};

		VkExtent3D			_extent;
		//TODO -> _written shouldn't exist, it should be up to the render graph.
		bool				_written;

		std::unordered_map<ViewConfig,
					VkImageView,
					ViewConfigHasher>	_views;
		std::unordered_map<VkImageView,
					VkDescriptorSet>	_textures;

		VkImageLayout	_currentLayout{VK_IMAGE_LAYOUT_UNDEFINED};
		VmaAllocation	_allocation{VK_NULL_HANDLE};
		VkImage			_image{VK_NULL_HANDLE};

	friend class ImagePool;
};

}

#include "api/vulkan/Image.tpp"
