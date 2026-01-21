/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 14:42:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/21 18:17:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "Registry.hpp"
#include <iostream>

namespace	hel {

template <typename Component, typename... Args>
Component	&Registry::addComponent(EntityId entity, Args&&... args) {
	Pool<Component>	&pool = getPool<Component>();
	if (pool.sparseArray.size() <= entity) {
		pool.sparseArray.resize(entity + 1, NOT_REGISTERED);
	}
	if (pool.sparseArray[entity] != NOT_REGISTERED) {
		std::cout << "Cannot add a component when one already exists. " <<
			"Use getComponent for that." << std::endl;
		return (pool.denseArray[pool.sparseArray[entity]]);
	}
	Component	&component = pool.denseArray.emplace_back(std::forward<Args>(args)...);
	pool.sparseArray[entity] = pool.denseArray.size() - 1;
	prepareComponent(component);
	return (component);
}

template <typename Component>
Component	&Registry::getComponent(EntityId entity) {
	Pool<Component>	&pool = getPool<Component>();
	return (pool.denseArray[pool.sparseArray[entity]]);
}

template <typename Component>
Component	*Registry::tryGetComponent(EntityId entity) {
	Pool<Component>	&pool = getPool<Component>();
	if (pool.sparseArray.size <= entity || pool.sparseArray[entity] == NOT_REGISTERED)
		return (nullptr);
	return (&pool.denseArray[pool.sparseArray[entity]]);
}

template <typename Component>
Pool<Component>	&Registry::getPool() {
	std::type_index	typeKey = typeid(Component);

	auto	pool = _pools.find(typeKey);
	if (pool == _pools.end()) {
		_pools[typeKey] = std::make_unique<Pool<Component>>();
		return (static_cast<Pool<Component>&>(*_pools[typeKey]));
	}
	return (static_cast<Pool<Component>&>(*pool->second));
}

template<typename Component>
void	Registry::prepareComponent(Component &component) {
	if constexpr (requires { component.init(_assetManager); }) {
		component.init(_assetManager);
	}
}

}
