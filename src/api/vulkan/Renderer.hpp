/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:48:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/04 15:44:56                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "utils/Setters.hpp"
#include "api/vulkan/PipelineMap.hpp"

namespace	hel::sys {

struct	PhaseDependencies;
struct	ImageDep;

}

namespace	hel {

class	Image;
class	Renderer;
class	Device;
struct	FrameContext;
struct	RenderRequest;
class	ImagePool;

class	RenderPass {
	public:
		RenderPass(Device &device, FrameContext &ctx, VkExtent2D extent);
		RenderPass(Device &device, FrameContext &context, ImagePool *imagePool,
			const std::vector<sys::ISystem *> &systems,
			sys::PhaseDependencies sys::ISystem::*depMember);
		RenderPass(Device &device, FrameContext &context, ImagePool *imagePool,
			sys::PhaseDependencies dep);
		RenderPass(RenderPass &&other);
		~RenderPass(void);

		SETTER(ColorLoadOp, VkAttachmentLoadOp, _colorsLoadOp)
		SETTER(ColorStoreOp, VkAttachmentStoreOp, _colorsStoreOp)
		SETTER(DepthLoadOp, VkAttachmentLoadOp, _depthLoadOp)
		SETTER(DepthStoreOp, VkAttachmentStoreOp, _depthStoreOp)
		SETTER(ClearValue, VkClearValue, _colorClear)
		RenderPass		&addColorWrite(Image *color, VkFormat format);
		RenderPass		&addDepthWrite(Image *depth, VkFormat format);

		Renderer		beginPass(void);

		static void	newFrame(void)	{ _passIndex = 0; }

	private:
		bool	addWrite(sys::ImageDep &dep, ImagePool *imagePool);
		bool	findUsage(sys::ImageDep &dep);
		bool	findLoadOp(sys::ImageDep &dep);
		bool	findStoreOp(sys::ImageDep &dep);
		void	addWriteImage(Image *img, sys::ImageDep &dep);

		bool	addRead(sys::ImageDep &dep);

		void	setViewport(void);
		void	endPass(void);

		Device				&_device;
		FrameContext		&_ctx;
		RenderRequest		*&_req;
		VkCommandBuffer		_commandBuffer;
		VkExtent2D			_extent;
		bool				_invalidDep{false};
		bool				_passStarted{false};

		VkClearValue			_colorClear{ .color = {{0.f, 0.f, 0.f, 1.0f}} };
		VkClearValue			_depthClear{ .depthStencil = {1.0f, 0} };
		VkAttachmentLoadOp		_colorsLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
		VkAttachmentStoreOp		_colorsStoreOp{VK_ATTACHMENT_STORE_OP_STORE};
		VkAttachmentLoadOp		_depthLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
		VkAttachmentStoreOp		_depthStoreOp{VK_ATTACHMENT_STORE_OP_DONT_CARE};

		std::unordered_map<std::string, Image *>	_writes{};
		std::unordered_map<std::string, Image *>	_reads{};

		Image										*_depthWrite;
		std::vector<VkRenderingAttachmentInfo>		_colorsInfo{};
		std::optional<VkRenderingAttachmentInfo>	_depthInfo{};
		std::optional<VkRenderingAttachmentInfo>	_stencilInfo{};
		RenderingConfig								_config;

		static uint32_t		_passIndex;
		static uint32_t		newPass(void)	{ return (_passIndex++); }

	friend class	Renderer;
};

class Renderer {
	public:
		explicit Renderer(FrameContext &frameContext, RenderPass &&pass);
		explicit operator	bool(void) const;

		FrameContext		&frameContext(void) const;
		uint32_t			passIndex(void) const;

		PASSKEY(ISystemKey, sys::ISystem)
		struct	Draw;
		Draw	drawCommand(PipelineMap *pipeline, ISystemKey) const;

	private:
		Device				&_device;
		FrameContext		&_frameContext;
		VkCommandBuffer		_commandBuffer;
		RenderingConfig		_config;

		RenderPass			_pass;
	friend struct	Draw;
};

struct	Renderer::Draw {
	Draw	&addBinding(VkDescriptorSet set);
	Draw	&addDynamicBinding(VkDescriptorSet set, uint32_t stride,
							uint32_t *offset, VkBufferUsageFlags setUsage =
							VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	template <typename T>
	Draw	&addPush(VkShaderStageFlags stage, const T &data);
	template <size_t N>
	Draw	&addVertexBuffers(const VkBuffer (&buffers)[N],
							const VkDeviceSize (&offsets)[N]);
	Draw	&addIndexBuffer(VkBuffer buffer, uint32_t firstIndex = 0,
							VkIndexType indexType = VK_INDEX_TYPE_UINT32,
							VkDeviceSize offset = 0);
	SETTER(VertexCount, uint32_t, _count)
	void	submit(void);

	private:
		Draw(const Renderer *renderer, PipelineMap *pipeline);

		struct	PushInfos {
			VkShaderStageFlags	stage;
			uint32_t			structSize;
			uint8_t				data[128];
		};
		struct	VertexInfos {
			uint32_t			bufferCount;
			VkBuffer			buffers[8];
			VkDeviceSize		offsets[8];
		};
		struct	IndexInfos {
			VkBuffer		buffer;
			VkDeviceSize	offset;
			VkIndexType		indexType;
		};

		PipelineMap						*_pipeline;
		Device							*_device;
		FrameContext					*_frameContext;
		VkCommandBuffer					_commandBuffer;
		RenderingConfig					_config;
		std::vector<VkDescriptorSet>	_sets{};
		std::vector<uint32_t>			_setsOffsets{};
		VertexInfos						_vertexInfos;
		bool							_hasVertex{false};
		IndexInfos						_indexInfos;
		bool							_hasIndex{false};
		PushInfos						_pushInfos;
		bool							_hasPush{false};
		uint32_t						_firstIndex{0};
		std::optional<uint32_t>			_count;

		static PipelineMap				*_lastPipeline;

	friend class Renderer;
};

}

#include "api/vulkan/Renderer.tpp"
