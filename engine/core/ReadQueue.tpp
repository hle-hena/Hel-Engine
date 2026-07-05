/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ReadQueue.tpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:33:38 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 18:34:12                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/ReadQueue.hpp"
#include "api/vulkan/Buffer.hpp"

namespace	hel {

template <typename ReadType>
Read::Builder<ReadType>::Builder(uint32_t frameIndex) {
	_context.frameIndex = frameIndex;
}

template <typename ReadType>
Read::Context	Read::Builder<ReadType>::push(Device &device) {
	uint32_t	count = _request.extent.width * _request.extent.height *
						_request.extent.depth;
	_context.buffer = Buffer::create(device, sizeof(ReadType), count,
						VK_BUFFER_USAGE_TRANSFER_DST_BIT,
						VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
						VMA_ALLOCATION_CREATE_MAPPED_BIT |
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	if (!_context.buffer || !_request.srcImage)
		return (std::move(_context));
	_request.dstBuffer = _context.buffer.get();
	Read::Queue::_requests.push_back(_request);
	return (std::move(_context));
}

}
