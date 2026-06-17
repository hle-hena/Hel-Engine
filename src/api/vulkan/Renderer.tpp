/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/22 12:19:09 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/17 14:25:50                                        */
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

namespace	hel {

template <size_t N>
DrawCall	&DrawCall::addVertexBuffers(const VkBuffer (&buffers)[N],
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
DrawCall	&DrawCall::addPush(VkShaderStageFlags stage, const T &data) {
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
