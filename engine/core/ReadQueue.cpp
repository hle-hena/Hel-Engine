/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ReadQueue.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:32:28 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 18:33:18                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/ReadQueue.hpp"
#include "api/vulkan/Image.hpp"

namespace	hel {

std::vector<Read::Request>	Read::Queue::_requests = {};

void	Read::Queue::execute(VkCommandBuffer commandBuffer) {
	for (auto &req: _requests)
		req.srcImage->copyTo(commandBuffer, req.dstBuffer,
							req.offset, req.extent);
	_requests.clear();
}

}
