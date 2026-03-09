/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/25 13:16:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/09 14:19:23                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <memory>
# include <unordered_map>
# include <vulkan/vulkan.h>
# include "utils/mathUtils.hpp"

#include <iostream>

namespace	hel {

class	Device;

class Image {
	public:
		using ptr = std::unique_ptr<Image>;
		struct	Config {
			uint32_t				width, height;
			std::vector<VkFormat>	format{};
			VkImageUsageFlags		usage;
			VkMemoryPropertyFlags	properties;
			VkImageAspectFlags		aspectFlags;
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
		void	setData(void *data, VkDeviceSize size);
		void	copyTo(VkCommandBuffer commandBuffer, Image *dst);

		VkImage						getImage(void) const
			{ return (_image); }
		VkImageView					getView(VkFormat format) const
			{ return (_views.at(format)); }
		VkExtent2D					getExtent(void) const
			{ return {_config.width, _config.height}; }
		VkFormat					getFormat(void) const
			{ return (_config.format[0]); }
		VkDescriptorSet				getTexture(VkFormat format);
		VkDescriptorImageInfo		getDescriptorInfo(VkFormat format) const;
		VkRenderingAttachmentInfo	getRenderingInfo(VkClearValue clearValue,
													VkAttachmentLoadOp loadOp,
													VkAttachmentStoreOp storeOp,
													VkFormat format) const;

	private:
		Image(Device &device, const Config &config);
		Image(Device &device, VkImage img, VkFormat format, VkExtent2D extent);

		void	createImage(void);
		void	allocateMemory(void);
		void	createViews(void);
		void	createView(VkFormat format);

		bool							_owned{true};
		Device							&_device;
		Config							_config;
		VkImage							_image{VK_NULL_HANDLE};
		std::unordered_map<VkFormat,
				VkImageView,
				mathUtils::EnumHash>	_views;
		std::unordered_map<VkFormat,
				VkDescriptorSet,
				mathUtils::EnumHash>	_textures;
		VkDeviceMemory					_memory{VK_NULL_HANDLE};
		VkImageLayout					_currentLayout{VK_IMAGE_LAYOUT_UNDEFINED};
};

}
