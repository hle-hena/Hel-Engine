/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ReadQueue.tpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:33:38 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 14:38:41                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/scheduler/ReadQueue.hpp"
#include "rhi/resources/Buffer.hpp"

namespace	hel {

template <typename ReadType>
Read::Builder<ReadType>::Builder(uint32_t frameIndex) {
	_context.frameIndex = frameIndex;
}

template <typename ReadType>
expected<Read::Context>	Read::Builder<ReadType>::push(Device *device) {
	if (!_request.srcImage)
		return unexpected("Missing a valid image for the read request");
	uint32_t	count = _request.extent.width * _request.extent.height *
						_request.extent.depth;
	auto	dstBuffer = Buffer::create<ReadType>(device, BufferConfig()
							.usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT)
							.allocFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
							.fixedCount(count));
	_context.buffer = dstBuffer;
	_request.dstBuffer = _context.buffer.get();
	Read::Queue::_requests.push_back(_request);
	return (_context);
}

}
