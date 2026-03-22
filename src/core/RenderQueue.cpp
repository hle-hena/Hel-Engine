/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/21 19:38:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/21 19:46:44                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/RenderQueue.hpp"

namespace	hel {

std::vector<RenderRequest>	RenderQueue::_requests = {};

void	RenderQueue::push(const RenderRequest &request) {
	_requests.push_back(request);
}

std::vector<RenderRequest>	RenderQueue::flush(void) {
	return (std::move(_requests));
}

}
