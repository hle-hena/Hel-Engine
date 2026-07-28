/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderRequest.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/28 15:32:12 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/28 19:49:18                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/render/RenderRequest.hpp"

namespace	std {

size_t	hash<hel::RenderRequest>::operator()(const hel::RenderRequest &request) const
{
	// Tag is not used in the == or the hashing because it is not comparable
	// in any meaningfull way.
	size_t	seed = 0;
	hel::mathUtils::hashCombine(seed,
		request._origin.width,
		request._origin.height,
		request._extent.width,
		request._extent.height,
		request._requestType
	);

	size_t imageSeed = 0;
	for (auto &[imageName, img] : request._images) {
		size_t entryHash = 0;
		hel::mathUtils::hashCombineMurmur(entryHash, imageName);
		imageSeed += entryHash; 
	}

	hel::mathUtils::hashCombine(seed, imageSeed);

	return seed;
}

}

namespace	hel {

bool	RenderRequest::operator==(const RenderRequest &other) const {
	// Tag is not used in the == or the hashing because it is not comparable
	// in any meaningfull way.

	bool	sameOrigin = (other._origin.width == this->_origin.width &&
						other._origin.height == this->_origin.height);
	bool	sameExtent = (other._extent.width == this->_extent.width &&
						other._extent.height == this->_extent.height);
	bool	sameRequestType = (other._requestType == this->_requestType);
	bool	containsSameImages = true;
	if (this->_images.size() != other._images.size())
		containsSameImages = false;
	else {
		for (auto &[imageName, image]: this->_images) {
			if (!other._images.contains(imageName)) {
				containsSameImages = false;
				break ;
			}
			if (this->_images.at(imageName) != other._images.at(imageName)) {
				containsSameImages = false;
				break;
			}
		}
	}

	return (sameOrigin && sameExtent
		&& sameRequestType && containsSameImages);
}

}
