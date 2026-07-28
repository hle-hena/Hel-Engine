/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:26:37 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/28 18:37:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/scheduler/RenderQueue.hpp"
#include "rhi/render/RenderRequest.hpp"

namespace	hel {

std::vector<RenderRequest>	RenderQueue::_requests = {};

void	RenderQueue::push(RenderRequest &&request) {
	_requests.push_back(std::move(request));
}

std::vector<RenderRequest>	RenderQueue::flush(void) {
	return (std::move(_requests));
}

}
