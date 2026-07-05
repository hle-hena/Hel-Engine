/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: DrawQueue.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:37:54 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 19:50:13                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/DrawQueue.hpp"

namespace	hel {

DrawQueue::RequestMap		DrawQueue::_requests = {};

DrawQueue::RequestVector	*DrawQueue::RequestMap::at(const uint32_t levelAsked, const PhaseDependencies &depAsked) {
	auto	&data = _data[levelAsked];
	for (auto &vector: data) {
		if (vector.dep == depAsked)
			return &vector;
	}
	auto	&newVec = data.emplace_back();
	newVec.dep = depAsked;
	return &newVec;
}

void	DrawQueue::requestDraw(uint32_t level, DrawCall &&drawCommand,
							PhaseDependencies &dep) {
	_requests.at(level, dep)->draws.emplace_back(std::move(drawCommand));
}

}
