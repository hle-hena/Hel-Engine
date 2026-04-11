/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/21 19:38:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/11 17:36:14                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/RenderQueue.hpp"
#include "utils/mathUtils.hpp"
#include <algorithm>

namespace	hel {

std::vector<RenderRequest>	RenderQueue::_requests = {};

void	RenderQueue::push(const RenderRequest &request) {
	_requests.push_back(request);
}

std::vector<RenderRequest>	RenderQueue::flush(void) {
	return (std::move(_requests));
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
	bool	sameImages = compImages(this->mainImage, other.mainImage);
	if (this->secondaryImages.size() == other.secondaryImages.size()) {
		sameImages &= std::equal(
			this->secondaryImages.begin(),
			this->secondaryImages.end(),
			other.secondaryImages.begin(),
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
	hashImage(seed, request.mainImage);
	for (auto it = request.secondaryImages.begin();
		it != request.secondaryImages.end(); it++)
		hashImage(seed, it->second);
	return (seed);
}

}
