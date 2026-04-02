/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Image.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/25 13:16:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/01 20:32:07                                        */
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
# include "utils/Setters.hpp"
# include "api/vulkan/vma/vk_mem_alloc.h"

namespace	hel {

class	Device;
class	Buffer;

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
		void	setData(void *data, VkDeviceSize size);
		void	copyTo(VkCommandBuffer commandBuffer, Image *dst);
		void	copyTo(VkCommandBuffer commandBuffer, Buffer *dst,
					VkOffset3D startPos, VkExtent3D extent);

		PASSKEY(ExtentKey, ImagePool)
		void	setExtent(const VkExtent2D &extent, ExtentKey)
			{ _extent = extent; }

		VkImage						getImage(void) const
			{ return (_image); }
		VkImageView					getView(VkFormat format) const
			{ return (_views.at(format)); }
		VkExtent2D					getExtent(void) const
			{ return (_extent); }
		VkExtent2D					getPhysicalExtent(void) const
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
		void	createViews(void);
		void	createView(VkFormat format);

		bool							_owned{true};
		Device							&_device;
		Config							_config;
		VkExtent2D						_extent;
		VkImage							_image{VK_NULL_HANDLE};
		std::unordered_map<VkFormat,
				VkImageView,
				mathUtils::EnumHash>	_views;
		std::unordered_map<VkFormat,
				VkDescriptorSet,
				mathUtils::EnumHash>	_textures;
		VmaAllocation					_allocation{VK_NULL_HANDLE};
		VkImageLayout					_currentLayout{VK_IMAGE_LAYOUT_UNDEFINED};
};

}
