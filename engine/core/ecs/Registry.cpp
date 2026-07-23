/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 12:07:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/16 11:01:35                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/Registry.hpp"
#include "ecs/Hierarchy.hpp"

namespace	hel {

void	Registry::init(Device *device) {
	_assetManager.init(device);
}

bool	Registry::isValidHandle(Entity::id handle) {
	uint32_t	entityIndex = Entity::getIndex(handle);
	return (entityIndex < _aliveEntities.size() &&
			_aliveEntities[entityIndex] == handle);
}

Entity::id	Registry::createEntity(void) {
	Entity::id	newEntity = Entity::acquire();
	uint32_t	newEntityIndex = Entity::getIndex(newEntity);
	if (_aliveEntities.size() <= newEntityIndex)
		_aliveEntities.resize(newEntityIndex + 1, Entity::NOT_REGISTERED);
	_aliveEntities[newEntityIndex] = newEntity;
	addComponent<comp::Hierarchy>(newEntity);
	return (newEntity);
}

template <>
void	Registry::removeComponent<comp::Hierarchy>(Entity::id) {
}

void	Registry::removeEntity(Entity::id handle) {
	if (!isValidHandle(handle))	{ return ; }
	
	uint32_t	entityIndex = Entity::getIndex(handle);
	_aliveEntities[entityIndex] = Entity::NOT_REGISTERED;
	for (auto &pool: _pools) {
		pool.second->removeEntity(handle);
	}
	Entity::release(handle);
}

void	Registry::resetAllDirty(void) {
	for (auto &[type, pool]: _pools)
		pool->resetDirtyFlag();
}

void	Registry::updateBuffers(Device &device) {
	for (auto &[type, pool]: _pools) {
		pool->removePendingBuffers();
		if (auto res = pool->flushWrites(device); !res)
			std::cerr << "Failed to flush a pool: " << res.error() << std::endl;
	}
}

}
