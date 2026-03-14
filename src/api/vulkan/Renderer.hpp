/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:48:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 22:32:25                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <optional>
# include <vector>

# include "utils/Setters.hpp"
# include "api/vulkan/PipelineMap.hpp"

namespace	hel {

class	Image;
class	RendererHandle;

class	Renderer {
	public:
		Renderer(VkCommandBuffer commandBuffer, VkExtent2D extent);
		Renderer(Renderer &&other);
		~Renderer(void);

		SETTER(ColorLoadOp, VkAttachmentLoadOp, _colorsLoadOp)
		SETTER(ColorStoreOp, VkAttachmentStoreOp, _colorsStoreOp)
		SETTER(DepthLoadOp, VkAttachmentLoadOp, _depthLoadOp)
		SETTER(DepthStoreOp, VkAttachmentStoreOp, _depthStoreOp)
		Renderer		&addColorWrite(Image *color, VkFormat format);
		Renderer		&addDepthWrite(Image *depth, VkFormat format);

		RendererHandle	beginPass(void);

	private:
		void	setViewport(void);
		void	endPass(void);

		VkCommandBuffer		_commandBuffer;
		VkExtent2D			_extent;
		bool				_isValid{false};

		VkClearValue			_colorClear{{{0.f, 0.f, 0.f, 1.0f}}};
		VkClearValue			_depthClear{ .depthStencil = {1.0f, 0} };
		VkAttachmentLoadOp		_colorsLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
		VkAttachmentStoreOp		_colorsStoreOp{VK_ATTACHMENT_STORE_OP_STORE};
		VkAttachmentLoadOp		_depthLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
		VkAttachmentStoreOp		_depthStoreOp{VK_ATTACHMENT_STORE_OP_DONT_CARE};

		std::vector<Image *>						_colorsWrite{};
		std::vector<Image *>						_colorsRead{};
		Image										*_depthWrite;
		std::vector<VkRenderingAttachmentInfo>		_colorsInfo{};
		std::optional<VkRenderingAttachmentInfo>	_depthInfo{};
		RenderingConfig								_config;
	
	friend class	RendererHandle;
};

class RendererHandle {
	public:
		explicit RendererHandle(Renderer &&renderer);
		explicit operator	bool(void) const;
		
		RenderingConfig		_config;

	private:
		Renderer			_renderer;
};

}
