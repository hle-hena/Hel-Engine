/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 14:42:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/23 18:52:29                                        */
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
	uint32_t		removedIndex = indices[entity];
	if (indices[entity] != lastIndex) {
		components[removedIndex] = std::move(components[lastIndex]);
		indices[entities[lastIndex]] = removedIndex;
		entities[removedIndex] = entities[lastIndex];
	}
	components.resize(lastIndex);
	entities.resize(lastIndex);
	indices[entity] = NOT_REGISTERED;
}

template <typename Component>
void	Pool<Component>::tryRemoveEntity(EntityId entity) {
	if (entity >= indices.size() || indices[entity] == NOT_REGISTERED)
		return;
	std::cout << "\r\033[31mRemoving an entity\033[0m\n" << std::endl;
	uint32_t		lastIndex = components.size() - 1;
	uint32_t		removedIndex = indices[entity];
	if (indices[entity] != lastIndex) {
		components[removedIndex] = std::move(components[lastIndex]);
		indices[entities[lastIndex]] = removedIndex;
		entities[removedIndex] = entities[lastIndex];
	}
	components.resize(lastIndex);
	entities.resize(lastIndex);
	indices[entity] = NOT_REGISTERED;
}



template <typename Component, typename... Args>
const Component	&Registry::addComponent(EntityId entity, Args&&... args) {
	Pool<Component>	&pool = getPool<Component>();
	if (pool.indices.size() <= entity) {
		pool.indices.resize(entity + 1, NOT_REGISTERED);
	}
	Component	&component = pool.components.emplace_back(std::forward<Args>(args)...);
	pool.entities.push_back(entity);
	pool.indices[entity] = pool.components.size() - 1;
	prepareComponent(component);
	return (component);
}

template <typename Component, typename... Args>
const Component	&Registry::tryAddComponent(EntityId entity, Args&&... args) {
	Pool<Component>	&pool = getPool<Component>();
	if (pool.indices.size() <= entity) {
		pool.indices.resize(entity + 1, NOT_REGISTERED);
	}
	if (pool.indices[entity] != NOT_REGISTERED) {
		std::cout << "Cannot add a component when one already exists. " <<
			"Use tryGetComponent for that." << std::endl;
		return (pool.components[pool.indices[entity]]);
	}
	Component	&component = pool.components.emplace_back(std::forward<Args>(args)...);
	pool.entities.push_back(entity);
	pool.indices[entity] = pool.components.size() - 1;
	prepareComponent(component);
	return (component);
}

template <typename Component>
const Component	&Registry::getComponent(EntityId entity) {
	Pool<Component>	&pool = getPool<Component>();
	return (pool.components[pool.indices[entity]]);
}

template <typename Component>
const Component	*Registry::tryGetComponent(EntityId entity) {
	Pool<Component>	&pool = getPool<Component>();
	if (pool.indices.size() <= entity || pool.indices[entity] == NOT_REGISTERED)
		return (nullptr);
	return (&pool.components[pool.indices[entity]]);
}


template <typename Component, typename Func>
void	Registry::patch(EntityId entity, Func &&func) {
	const Component	*comp = tryGetComponent<Component>(entity);
	if (!comp)
		return ;
	Component		&mutableComp = const_cast<Component &>(*comp);
	func(mutableComp);
	if constexpr (requires { mutableComp.isDirty = true; })
		mutableComp.isDirty = true;
}

template <typename Component, typename Func>
void	Registry::patch(const Component &comp, Func &&func) {
	Component	&mutableComp = const_cast<Component &>(comp);
	func(mutableComp);
	if constexpr (requires { mutableComp.isDirty = true; })
		mutableComp.isDirty = true;
}

template <typename Component, typename Func>
void	Registry::update(EntityId entity, Func &&func) {
	const Component	*comp = tryGetComponent<Component>(entity);
	if (!comp)
		return ;
	Component		&mutableComp = const_cast<Component &>(*comp);
	func(mutableComp);
	if constexpr (requires { mutableComp.isDirty = false; })
		mutableComp.isDirty = false;
}

template <typename Component, typename Func>
void	Registry::update(const Component &comp, Func &&func) {
	Component	&mutableComp = const_cast<Component &>(comp);
	func(mutableComp);
	if constexpr (requires { mutableComp.isDirty = false; })
		mutableComp.isDirty = false;
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
	static_assert(sizeof...(Components) > 0, "Cannot create an empty View. Please provide at least one component");
    static_assert(is_unique<Components...>::value, "View contains duplicate component types");
	return (View<Components...>(*this));
}

template<typename Component>
void	Registry::prepareComponent(Component &component) {
	if constexpr (requires { component.init(_assetManager); }) {
		component.init(_assetManager);
	}
}

}
