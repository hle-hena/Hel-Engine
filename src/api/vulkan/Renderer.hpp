/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:48:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/31 11:05:24                                        */
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
# include "api/vulkan/Descriptors.hpp"
# include "core/Frame.hpp"

namespace	hel {

class	Image;
class	Renderer;
class	Device;
struct	FrameContext;

class	RenderPass {
	public:
		RenderPass(Device &device, VkCommandBuffer commandBuffer, VkExtent2D extent);
		RenderPass(RenderPass &&other);
		~RenderPass(void);

		SETTER(ColorLoadOp, VkAttachmentLoadOp, _colorsLoadOp)
		SETTER(ColorStoreOp, VkAttachmentStoreOp, _colorsStoreOp)
		SETTER(DepthLoadOp, VkAttachmentLoadOp, _depthLoadOp)
		SETTER(DepthStoreOp, VkAttachmentStoreOp, _depthStoreOp)
		RenderPass		&addColorWrite(Image *color, VkFormat format);
		RenderPass		&addDepthWrite(Image *depth, VkFormat format);

		Renderer		beginPass(FrameContext &context);

		static void	newFrame(void)	{ _passIndex = 0; }

	private:
		void	setViewport(void);
		void	endPass(void);

		Device				&_device;
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

		static uint32_t		_passIndex;
		static uint32_t		newPass(void)	{ return (_passIndex++); }

	friend class	Renderer;
};

class Renderer {
	public:
		explicit Renderer(FrameContext &frameContext, RenderPass &&pass);
		explicit operator	bool(void) const;

		FrameContext		&frameContext(void) const	{ return (_frameContext); }
		uint32_t			passIndex(void) const	{ return (_frameContext.passIndex); }

		PASSKEY(ISystemKey, sys::ISystem)
		struct	Draw;
		Draw	drawCommand(PipelineMap *pipeline, ISystemKey) const;

	private:
		Device				&_device;
		FrameContext		&_frameContext;
		VkCommandBuffer		_commandBuffer;
		RenderingConfig		_config;

		RenderPass			_pass;
};

struct	Renderer::Draw {
	template <size_t N>
	Draw	&addVertexBuffers(const VkBuffer (&buffers)[N],
							const VkDeviceSize (&offsets)[N]);
	Draw	&addIndexBuffer(VkBuffer buffer, VkDeviceSize offset,
							VkIndexType indexType, uint32_t firstIndex = 0);
	Draw	&addBinding(VkDescriptorSet set);
	Draw	&addBinding(VkDescriptorSet set, uint32_t stride, uint32_t *offset);
	template <typename T>
	Draw	&addPush(VkShaderStageFlags stage, const T &data);
	void	submit(uint32_t indexCount, uint32_t instanceCount = 1,
				uint32_t firstInstance = 0);
	void	submitNoVertex(uint32_t indexCount, uint32_t instanceCount = 1,
				uint32_t firstInstance = 0);

	private:
		Draw(Device &device, FrameContext &frameContext, VkCommandBuffer commandBuffer,
			VkPipelineLayout pipelineLayout)
			: _device{device}, _frameContext{frameContext},
				_commandBuffer{commandBuffer}, _pipelineLayout{pipelineLayout} {}

		Device							&_device;
		FrameContext					&_frameContext;
		VkCommandBuffer					_commandBuffer;
		VkPipelineLayout				_pipelineLayout;
		std::vector<VkDescriptorSet>	_sets{};
		std::vector<uint32_t>			_setsOffsets{};
		bool							_hasVertex{false};
		bool							_hasIndex{false};
		bool							_hasPush{false};
		uint32_t						_firstIndex{0};

	friend class Renderer;
};

}

#include "api/vulkan/Renderer.tpp"
