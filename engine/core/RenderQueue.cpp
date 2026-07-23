/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:26:37 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/23 10:40:48                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/RenderQueue.hpp"
#include "utils/mathUtils.hpp"
#include "api/vulkan/Image.hpp"

namespace	hel {

std::vector<RenderRequest>	RenderQueue::_requests = {};

bool	RenderRequest::operator==(const RenderRequest &other) const {
	bool	sameHandle = (other.handle == this->handle);
	bool	sameOrigin = (other.origin.x == this->origin.x &&
						other.origin.y == this->origin.y);

	auto	compImages = [&](const Ref<Image> l, const Ref<Image> r) -> bool {
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
	auto	hashImage = [&](size_t &seed, Ref<Image> img){
		auto	extent = img->getExtent();
		hel::mathUtils::hashCombine(seed, extent.width, extent.height);
	};
	for (auto it = request.images.begin();
		it != request.images.end(); it++)
		hashImage(seed, it->second);
	return (seed);
}

}
