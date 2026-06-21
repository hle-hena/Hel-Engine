/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:48:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/17 14:25:36                                        */
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

#include "utils/Setters.hpp"
#include "api/vulkan/PipelineMap.hpp"
#include "api/vulkan/RenderPass.hpp"

namespace hel::sys {

class	ISystem;

}

namespace	hel {

class	Image;
class	Renderer;
class	Device;
struct	FrameContext;
struct	RenderRequest;
class	ImagePool;

struct	DrawCall;

class Renderer {
	public:
		explicit Renderer(FrameContext &frameContext, RenderPass &&pass);
		explicit operator	bool(void) const;

		FrameContext		&frameContext(void) const;
		uint32_t			passIndex(void) const;

		PASSKEY(ISystemKey, sys::ISystem)
		DrawCall	drawCommand(PipelineMap *pipeline, ISystemKey) const;

	private:
		Device				&_device;
		FrameContext		&_frameContext;
		VkCommandBuffer		_commandBuffer;
		RenderingConfig		_config;

		RenderPass			_pass;
	friend struct	DrawCall;
};

struct	DrawCall {
	DrawCall	&addBinding(VkDescriptorSet set);
	DrawCall	&addDynamicBinding(VkDescriptorSet set, uint32_t stride,
							uint32_t *offset, VkBufferUsageFlags setUsage =
							VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	template <typename T>
	DrawCall	&addPush(VkShaderStageFlags stage, const T &data);
	template <size_t N>
	DrawCall	&addVertexBuffers(const VkBuffer (&buffers)[N],
							const VkDeviceSize (&offsets)[N]);
	DrawCall	&addIndexBuffer(VkBuffer buffer, uint32_t firstIndex = 0,
							VkIndexType indexType = VK_INDEX_TYPE_UINT32,
							VkDeviceSize offset = 0);
	SETTER(VertexCount, uint32_t, _count)
	void	submit(void);

	private:
		DrawCall(const Renderer *renderer, PipelineMap *pipeline);

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
