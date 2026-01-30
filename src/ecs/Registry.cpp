/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 12:07:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/30 15:29:16                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "Registry.hpp"

namespace	hel {

Registry::Registry(AssetManager &assetManager)
	:	_assetManager{assetManager} {
	
}

void	Registry::removeEntity(Entity::id handle) {
	for (auto &pool: _pools) {
		pool.second->tryRemoveEntity(handle);
	}
}

}
