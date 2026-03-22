/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/22 12:19:09 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/22 12:53:53                                        */
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

}
