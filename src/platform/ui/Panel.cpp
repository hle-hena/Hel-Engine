/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Panel.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 12:54:54 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/16 13:30:49                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/Panel.hpp"
#include "platform/ui/Dock.hpp"

namespace	hel::sys {

void	IPanel::setup(Registry *registry, ImagePool *imagePool) {
	_registry = registry;
	_imagePool = imagePool;
};

void	IPanel::changeOwner(Dock *newOwner) {
	auto	it = std::find(_owner->_panels.begin(),
						_owner->_panels.end(), this);
	if (it != _owner->_panels.end())	{ _owner->_panels.erase(it); }
	_owner = newOwner;
	_owner->_panels.push_back(this);
}

void	IPanel::setOwner(Dock *newOwner) {
	_owner = newOwner;
	_owner->_panels.push_back(this);
}

}
