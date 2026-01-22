/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 14:42:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/22 19:55:58                                        */
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

template <typename Component>
void	Pool<Component>::removeEntity(EntityId entity) {
	uint32_t		lastIndex = components.size() - 1;
	uint32_t		removedIndex = entityToIndex[entity];
	if (entityToIndex[entity] != lastIndex) {
		components[removedIndex] = std::move(components[lastIndex]);
		EntityId	lastEntity = indexToEntity[lastIndex];
		entityToIndex[lastEntity] = removedIndex;
		indexToEntity[removedIndex] = indexToEntity[lastIndex];
	}
	components.resize(lastIndex);
	indexToEntity.resize(lastIndex);
	entityToIndex[entity] = NOT_REGISTERED;
}

template <typename Component>
void	Pool<Component>::tryRemoveEntity(EntityId entity) {
	if (entity >= entityToIndex.size() || entityToIndex[entity] == NOT_REGISTERED)
		return;
	uint32_t		lastIndex = components.size() - 1;
	uint32_t		removedIndex = entityToIndex[entity];
	if (entityToIndex[entity] != lastIndex) {
		components[removedIndex] = std::move(components[lastIndex]);
		EntityId	lastEntity = indexToEntity[lastIndex];
		entityToIndex[lastEntity] = removedIndex;
		indexToEntity[removedIndex] = indexToEntity[lastIndex];
	}
	components.resize(lastIndex);
	indexToEntity.resize(lastIndex);
	entityToIndex[entity] = NOT_REGISTERED;
}



template <typename Component, typename... Args>
Component	&Registry::addComponent(EntityId entity, Args&&... args) {
	Pool<Component>	&pool = getPool<Component>();
	if (pool.entityToIndex.size() <= entity) {
		pool.entityToIndex.resize(entity + 1, NOT_REGISTERED);
	}
	Component	&component = pool.components.emplace_back(std::forward<Args>(args)...);
	pool.indexToEntity.push_back(entity);
	pool.entityToIndex[entity] = pool.components.size() - 1;
	prepareComponent(component);
	return (component);
}

template <typename Component, typename... Args>
Component	&Registry::tryAddComponent(EntityId entity, Args&&... args) {
	Pool<Component>	&pool = getPool<Component>();
	if (pool.entityToIndex.size() <= entity) {
		pool.entityToIndex.resize(entity + 1, NOT_REGISTERED);
	}
	if (pool.entityToIndex[entity] != NOT_REGISTERED) {
		std::cout << "Cannot add a component when one already exists. " <<
			"Use tryGetComponent for that." << std::endl;
		return (pool.components[pool.entityToIndex[entity]]);
	}
	Component	&component = pool.components.emplace_back(std::forward<Args>(args)...);
	pool.indexToEntity.push_back(entity);
	pool.entityToIndex[entity] = pool.components.size() - 1;
	prepareComponent(component);
	return (component);
}

template <typename Component>
Component	&Registry::getComponent(EntityId entity) {
	Pool<Component>	&pool = getPool<Component>();
	return (pool.components[pool.entityToIndex[entity]]);
}

template <typename Component>
Component	*Registry::tryGetComponent(EntityId entity) {
	Pool<Component>	&pool = getPool<Component>();
	if (pool.entityToIndex.size() <= entity || pool.entityToIndex[entity] == NOT_REGISTERED)
		return (nullptr);
	return (&pool.components[pool.entityToIndex[entity]]);
}

template <typename Component, typename Func>
void	Registry::patch(EntityId entity, Func &&func) {
	Component	&comp = tryGetComponent<Component>(entity);
	func(comp);
	if constexpr (requires { comp.isDirty = true; })
		comp.isDirty = true;
}

template <typename Component, typename Func>
void	Registry::patch(Component &comp, Func &&func) {
	func(comp);
	if constexpr (requires { comp.isDirty = true; })
		comp.isDirty = true;
}


template <typename Component>
void	Registry::removeComponent(EntityId entity) {
	Pool<Component>	&pool = getPool<Component>();
	pool.removeEntity(entity);
}

template <typename Component>
void	Registry::tryRemoveComponent(EntityId entity) {
	Pool<Component>	&pool = getPool<Component>();
	pool.tryRemoveEntity(entity);
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

template <typename... Components>
View<Components...> Registry::view() {
    return View<Components...>(*this);
}

template<typename Component>
void	Registry::prepareComponent(Component &component) {
	if constexpr (requires { component.init(_assetManager); }) {
		component.init(_assetManager);
	}
}

}
