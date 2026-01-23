/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 12:07:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/22 12:26:23                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "Registry.hpp"

namespace	hel {

void	Registry::removeEntity(EntityId entity) {
	for (auto &pool: _pools) {
		pool.second->tryRemoveEntity(entity);
	}
}

}
