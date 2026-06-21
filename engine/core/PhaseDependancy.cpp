/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PhaseDependancy.cpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/05 12:15:13 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/17 10:50:59                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "core/PhaseDependancy.hpp"
#include "utils/mathUtils.hpp"

namespace hel {

PhaseDependencies	*ImageDep::addDep(void) {
	_parent->write.push_back(*this);
	return _parent;
}

size_t	DepHasher::operator()(const PhaseDependencies &dep) const {
	size_t	seed = 0;
	for (auto &write: dep.write)
		mathUtils::hashCombine(seed, write._imageName,
							write._usage, write._format);
	for (auto &readName: dep.read)
		mathUtils::hashCombine(seed, readName);
	return seed;
}

bool	ImageDep::operator==(const ImageDep &o) const {
	return (_imageName == o._imageName
			&& _usage == o._usage
			&& _format == o._format);
}

bool	PhaseDependencies::operator==(const PhaseDependencies &o) const {
	return write == o.write && read == o.read;
}

}
