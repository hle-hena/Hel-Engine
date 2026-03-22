/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:48:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/22 13:15:59                                        */
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

		PASSKEY(ISystemKey, sys::ISystem)
		bool	bindPipeline(PipelineMap *pipeline, ISystemKey) const;
		struct	Draw;
		Draw	drawCommand(ISystemKey) const;

	private:
		RenderingConfig		_config;
		Renderer			_renderer;
};

struct	RendererHandle::Draw {
	template <size_t N>
	Draw	&addVertexBuffers(const VkBuffer (&buffers)[N],
							const VkDeviceSize (&offsets)[N]);
	Draw	&addIndexBuffer(VkBuffer buffer, VkDeviceSize offset,
							VkIndexType indexType, uint32_t firstIndex = 0);
	void	submit(uint32_t indexCount, uint32_t instanceCount = 1,
				uint32_t firstInstance = 0);

	private:
		Draw(VkCommandBuffer commandBuffer) : _commandBuffer{commandBuffer} {}

		VkCommandBuffer	_commandBuffer;
		bool			_hasVertex{false};
		bool			_hasIndex{false};
		uint32_t		_firstIndex{0};

	friend class RendererHandle;
};

}

#include "api/vulkan/Renderer.tpp"
