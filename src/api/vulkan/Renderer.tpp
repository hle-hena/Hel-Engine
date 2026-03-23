/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/22 12:19:09 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/23 16:52:30                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Renderer.hpp"

namespace	hel {

template <size_t N>
RendererHandle::Draw	&RendererHandle::Draw::addVertexBuffers(const VkBuffer (&buffers)[N],
								const VkDeviceSize (&offsets)[N]) {
	if (_hasVertex)
		return (*this);
	vkCmdBindVertexBuffers(_commandBuffer, 0, N, buffers, offsets);
	_hasVertex = true;
	return (*this);
}

template <typename T>
RendererHandle::Draw	&RendererHandle::Draw::addPush(VkShaderStageFlags stage, const T &data) {
	if (_hasPush)
		return (*this);
	vkCmdPushConstants(_commandBuffer, _pipelineLayout,
					stage, 0, sizeof(T), &data);
	_hasPush = true;
	return (*this);
}

}
