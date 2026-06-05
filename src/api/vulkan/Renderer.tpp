/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/22 12:19:09 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/05 12:39:40                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Renderer.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>
#include "core/Frame.hpp"
#include "core/Queues.hpp"

namespace	hel {

template <std::ranges::input_range R>
RenderPass::RenderPass(Device &device, FrameContext &ctx, ImagePool *imagePool,
			R &systems, PhaseDependencies sys::ISystem::*depMember)
	:	_device{device},
		_ctx{ctx},
		_req{ctx.request},
		_commandBuffer{ctx.commandBuffer},
		_extent{ctx.request->images["mainColor"]->getExtent()} {
	for (auto &system: systems) {
		for (auto &dep: (system->*depMember).write)
			_invalidDep |= addWrite(dep, imagePool);
		for (auto &dep: (system->*depMember).read)
			_invalidDep |= addRead(dep);
	}
}

template <size_t N>
Renderer::Draw	&Renderer::Draw::addVertexBuffers(const VkBuffer (&buffers)[N],
								const VkDeviceSize (&offsets)[N]) {
	if (N >= 8) {
		std::cerr << "Max allowed vertex buffers: 8\n";
		return (*this);
	}
	if (_hasVertex)
		return (*this);
	_vertexInfos.bufferCount = N;
	for (size_t i = 0; i < N; i++) {
		_vertexInfos.buffers[i] = buffers[i];
		_vertexInfos.offsets[i] = offsets[i];
	}
	_hasVertex = true;
	return (*this);
}

template <typename T>
Renderer::Draw	&Renderer::Draw::addPush(VkShaderStageFlags stage, const T &data) {
	uint32_t	structSize = sizeof(T);
	if (structSize > 128) {
		std::cerr << "Please don't use push constant for big structs." <<
				" Create a component and use the SSBO.\n";
		return (*this);
	}
	if (_hasPush)
		return (*this);
	_pushInfos.stage = stage;
	_pushInfos.structSize = structSize;
	memcpy(_pushInfos.data, &data, structSize);
	_hasPush = true;
	return (*this);
}

}
