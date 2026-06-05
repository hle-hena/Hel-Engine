/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PhaseDependancy.cpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/05 12:15:13 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/05 12:17:48                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "core/PhaseDependancy.hpp"

namespace hel {

size_t	DepHasher::operator()(const PhaseDependencies &dep) const {
	size_t	seed = 0;
	for (auto &write: dep.write)
		mathUtils::hashCombine(seed, write.imageName, write.usage, write.format);
	for (auto &read: dep.read)
		mathUtils::hashCombine(seed, read.imageName, read.usage, read.format);
	return seed;
}

bool	ImageDep::operator==(const ImageDep &o) const {
	return imageName == o.imageName && usage == o.usage && format == o.format;
}

bool	PhaseDependencies::operator==(const PhaseDependencies &o) const {
	return write == o.write && read == o.read;
	//TODO -> actually, check if there is no overlap on the order of the writes,
	// and if there isn't a write image in the read
}

}
