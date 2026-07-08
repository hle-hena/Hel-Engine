/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Panel.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 12:54:54 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/27 16:26:36                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "systems/ui/Panel.hpp"
#include "systems/ui/Dock.hpp"

#include <algorithm>

namespace	hel::sys {

expected<void>	IPanel::setup(Registry *registry, ImagePool *imagePool) {
	_registry = registry;
	_imagePool = imagePool;
	auto res = onInit();
	if (!res)
		return unexpected(res.error());
	return {};
};

void	IPanel::changeOwner(Dock *newOwner, size_t insertIdx) {
	insertIdx = std::clamp(insertIdx, static_cast<size_t>(0), newOwner->_panels.size());
	newOwner->_panels.insert(newOwner->_panels.begin() + static_cast<uint32_t>(insertIdx), this);
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
