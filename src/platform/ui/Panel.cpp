/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Panel.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 12:54:54 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/18 17:24:52                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/Panel.hpp"
#include "platform/ui/Dock.hpp"

#include <algorithm>

namespace	hel::sys {

void	IPanel::setup(Registry *registry, ImagePool *imagePool) {
	_registry = registry;
	_imagePool = imagePool;
};

void	IPanel::changeOwner(Dock *newOwner, size_t insertIdx) {
	insertIdx = std::clamp(insertIdx, 0ul, newOwner->_panels.size());
	newOwner->_panels.insert(newOwner->_panels.begin() + insertIdx, this);
	if (_owner) {
		auto	it = std::find(_owner->_panels.begin(),
							_owner->_panels.end(), this);
		if (it != _owner->_panels.end())	{ _owner->_panels.erase(it); }
	}
	_owner = newOwner;
}

void	IPanel::setOwner(Dock *newOwner) {
	if (!_owner) {
		_owner = newOwner;
		_owner->_panels.push_back(this);
		return ;
	}
	changeOwner(newOwner);
}

}
