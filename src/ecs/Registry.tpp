/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 14:42:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/04 18:31:08                                        */
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
void	Pool<Component>::removeEntity(Entity::id handle) {
	uint32_t		entityIndex = Entity::getIndex(handle);
	if (entityIndex >= indices.size() || indices[entityIndex] == Entity::NOT_REGISTERED)
		return;
	uint32_t		lastIndex = components.size() - 1;
	uint32_t		removedIndex = indices[entityIndex];
	if (removedIndex != lastIndex) {
		components[removedIndex] = std::move(components[lastIndex]);
		indices[Entity::getIndex(entities[lastIndex])] = removedIndex;
		entities[removedIndex] = entities[lastIndex];
	}
	components.resize(lastIndex);
	entities.resize(lastIndex);
	indices[entityIndex] = Entity::NOT_REGISTERED;
}

template <typename Component>
void	Pool<Component>::resetDirtyFlag(void) {
	if constexpr (requires(Component c) { c.isDirty = false; }) {
		for (auto &comp : components) {
			comp.isDirty = false;
		}
	}
}



template <typename Component, typename... Args>
const Component	*Registry::addComponent(Entity::id handle, Args&&... args) {
	if (!isValidHandle(handle))	{ return (nullptr); }
	Pool<Component>	&pool = getPool<Component>();
	uint32_t		entityIndex = Entity::getIndex(handle);
	if (pool.indices.size() <= entityIndex) {
		pool.indices.resize(entityIndex + 1, Entity::NOT_REGISTERED);
	}
	if (pool.indices[entityIndex] != Entity::NOT_REGISTERED) {
		std::cout << "Cannot add a component when one already exists. " <<
			"Use getComponent to get it and modify to modifiy it." << std::endl;
		return (&pool.components[pool.indices[entityIndex]]);
	}
	Component	&component = pool.components.emplace_back(std::forward<Args>(args)...);
	pool.entities.push_back(handle);
	pool.indices[entityIndex] = pool.components.size() - 1;
	prepareComponent(component);
	return (&component);
}

template <typename Component>
const Component	*Registry::getComponent(Entity::id handle) {
	if (!isValidHandle(handle))	{ return (nullptr); }
	Pool<Component>	&pool = getPool<Component>();
	uint32_t		entityIndex = Entity::getIndex(handle);
	if (pool.indices.size() <= entityIndex || pool.indices[entityIndex] == Entity::NOT_REGISTERED)
		return (nullptr);
	return (&pool.components[pool.indices[entityIndex]]);
}

template <typename Component>
void	Registry::removeComponent(Entity::id handle) {
	if (!isValidHandle(handle))	{ return ; }
	Pool<Component>	&pool = getPool<Component>();
	pool.tryRemoveEntity(handle);
}



template <typename Component>
ModificationProxy<Component>	Registry::modify(Entity::id handle) {
	auto	*comp = getComponent<Component>(handle);
	return (ModificationProxy<Component>(const_cast<Component *>(comp)));
}

template <typename Component>
ModificationProxy<Component>	Registry::modify(const Component *comp) {
	return (ModificationProxy<Component>(const_cast<Component *>(comp)));
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
