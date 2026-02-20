/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderPass.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/19 17:25:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/19 19:27:09                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <unordered_map>

namespace	hel {

class	Device;
class	Window;

class	RenderPass {
	public:
		RenderPass(Device &device);
		~RenderPass(void);
		RenderPass(const RenderPass &) = delete;
		RenderPass	&operator=(const RenderPass &) = delete;

		VkRenderPass	getRenderPasss(VkFormat imageFormat,
											VkFormat depthFormat);
		static void			beginRenderPass(VkRenderPass renderPass,
										VkCommandBuffer commandBuffer,
										VkFramebuffer frameBuffer,
										VkExtent2D extent);
		static void			endRenderPass(VkCommandBuffer commandBuffer);

	private:
		VkRenderPass	createRenderPass(VkFormat imageFormat,
										VkFormat depthFormat);

		Device										&_device;
		std::unordered_map<VkFormat, VkRenderPass>	_renderPasses;
};

}
