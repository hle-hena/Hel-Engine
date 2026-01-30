/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 12:07:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/30 16:28:36                                        */
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

bool	Registry::isValidHandle(Entity::id handle) {
	uint32_t	entityIndex = Entity::getIndex(handle);
	if (entityIndex < _aliveEntities.size() &&
			_aliveEntities[entityIndex] == handle)
		return (true);
	std::cout << "Nope, c'est pas valide ca" << std::endl;
	return (false);
}

Entity::id	Registry::createEntity(void) {
	Entity::id	newEntity = Entity::acquire();
	uint32_t	newEntityIndex = Entity::getIndex(newEntity);
	if (_aliveEntities.size() <= newEntityIndex)
		_aliveEntities.resize(newEntityIndex + 1, Entity::NOT_REGISTERED);
	_aliveEntities[newEntityIndex] = newEntity;
	return (newEntity);
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

}
