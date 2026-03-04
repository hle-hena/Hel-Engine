/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/25 13:16:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/04 18:12:16                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <memory>
# include <vulkan/vulkan.h>

namespace	hel {

class	Device;

class Image {
	public:
		using ptr = std::unique_ptr<Image>;
		struct	Config {
			uint32_t				width, height;
			VkFormat				format;
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

		VkDescriptorImageInfo	getDescriptorInfo(void) const
			{ return {nullptr, _view, _currentLayout}; };
		VkImage					getImage(void) const
			{ return (_image); }
		VkImageView				getView(void) const
			{ return (_view); }
		VkExtent2D				getExtent(void) const
			{ return {_config.width, _config.height}; }
		VkRenderingAttachmentInfo	getRenderingInfo(VkClearValue clearValue,
				VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp) const;

	private:
		Image(Device &device, const Config &config);
		Image(Device &device, VkImage img, VkFormat format, VkExtent2D extent);

		void	createImage(void);
		void	allocateMemory(void);
		void	createView(void);

		bool			_owned{true};
		Device			&_device;
		Config			_config;
		VkImage			_image{VK_NULL_HANDLE};
		VkImageView		_view{VK_NULL_HANDLE};
		VkDeviceMemory	_memory{VK_NULL_HANDLE};
		VkImageLayout	_currentLayout{VK_IMAGE_LAYOUT_UNDEFINED};
};

}
