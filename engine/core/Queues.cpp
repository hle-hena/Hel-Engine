/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/21 19:38:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/15 17:49:41                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Queues.hpp"
#include "utils/mathUtils.hpp"
#include <algorithm>

namespace	hel {

std::vector<RenderRequest>	RenderQueue::_requests = {};
std::vector<Read::Request>	Read::Queue::_requests = {};
DrawQueue::RequestMap		DrawQueue::_requests = {};



void	Read::Queue::execute(VkCommandBuffer commandBuffer) {
	for (auto &req: _requests)
		req.srcImage->copyTo(commandBuffer, req.dstBuffer,
							req.offset, req.extent);
	_requests.clear();
}



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



bool	RenderRequest::operator==(const RenderRequest &other) const {
	bool	sameHandle = (other.handle == this->handle);
	bool	sameOrigin = (other.origin.x == this->origin.x &&
						other.origin.y == this->origin.y);

	auto	compImages = [&](const Image *l, const Image *r) -> bool {
		bool	sameSize = (l->getExtent().width == r->getExtent().width &&
							l->getExtent().height == r->getExtent().height);
		return (sameSize);
	};
	bool	sameImages = false;
	if (this->images.size() == other.images.size()) {
		sameImages = std::equal(
			this->images.begin(),
			this->images.end(),
			other.images.begin(),
			[&](const auto &l, const auto &r){
				return (compImages(l.second, r.second));
			}
		);
	} else
		sameImages = false;

	return (sameHandle && sameOrigin && sameImages);
}

size_t	RenderRequest::Hasher::operator()(const RenderRequest &request) const {
	size_t	seed = 0;
	hel::mathUtils::hashCombine(seed, request.handle, request.origin.x, request.origin.y);
	auto	hashImage = [&](size_t &seed, Image *img){
		auto	extent = img->getExtent();
		hel::mathUtils::hashCombine(seed, extent.width, extent.height);
	};
	for (auto it = request.images.begin();
		it != request.images.end(); it++)
		hashImage(seed, it->second);
	return (seed);
}

}
