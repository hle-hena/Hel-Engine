/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: IComponent.cpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/02 17:02:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 14:30:41                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/ecs/IComponent.hpp"

namespace	hel {

OpaqueComponentHandle::~OpaqueComponentHandle(void) {
	if (!_dismissed)
		_pool->markDirty(_index.value());
}

OpaqueComponentHandle::operator bool(void) const {
	return (_index.has_value());
}

void	*OpaqueComponentHandle::getRaw(void) {
	return _pool->getRaw(_index.value());
}

bool	OpaqueComponentHandle::isDirty(void) {
	return _pool->isDirty(_index.value());
}

void	OpaqueComponentHandle::dismiss(void) {
	_dismissed = true;
}

std::string_view	OpaqueComponentHandle::typeName(void) {
	return _pool->getTypeName();
}

}
