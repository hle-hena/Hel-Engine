/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 14:42:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/30 11:49:59                                        */
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
void	Pool<Component>::syncBuffer(Device &device) {
	if constexpr (requires { Component::gpuVisible == true; }) {
		uint32_t	nbComp = components.size();
		if (!buffer || buffer->getSize() < nbComp * sizeof(Component)) {
			buffer = Buffer::create(device, sizeof(Component),
						std::max(nbComp, 8u),
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
						VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
						VMA_ALLOCATION_CREATE_MAPPED_BIT);
		}
		buffer->writeToBuffer(components.data(), nbComp * sizeof(Component));
		isDirty = false;
	}
}

template <typename Component>
void	Pool<Component>::syncBuffer(Device &device, PendingWrite &write) {
	if constexpr (requires { Component::gpuVisible == true; }) {
		buffer->writeToBuffer(write.data, sizeof(Component), write.offset);
	}
}

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
	isDirty = true;
}

template <typename Component>
void	Pool<Component>::resetDirtyFlag(void) {
	if constexpr (requires(Component c) { c.isDirty = false; }) {
		for (auto &comp : components) {
			comp.isDirty = false;
		}
	}
}

template <typename Component>
void	Pool<Component>::addWrite(uint32_t offset, void *data) {
	_writes.push_back({.offset = offset, .data = data});
}

template <typename Component>
void	Pool<Component>::flushWrites(Device &device) {
	if (isDirty) {
		_writes.clear();
		return (syncBuffer(device));
	}
	for (auto &write: _writes) {
		syncBuffer(device, write);
	}
	_writes.clear();
}

template <typename Component>
bool	Pool<Component>::has(Entity::id handle) const {
	uint32_t	idx = Entity::getIndex(handle);
	return (idx < indices.size() && indices[idx] != Entity::NOT_REGISTERED);
}

template <typename Component>
void	*Pool<Component>::getRaw(Entity::id handle) {
	uint32_t	idx = Entity::getIndex(handle);
	return (&components[indices[idx]]);
}

template <typename Component>
const char	*Pool<Component>::getTypeName(void) const {
	if constexpr (requires { Component::label; })
		return (Component::label);
	return (typeid(Component).name());
}



template <typename Component, typename... Args>
ComponentHandle<Component>	Registry::addComponent(Entity::id entityHandle, Args&&... args) {
	ComponentHandle<Component>	compHandle;
	if (!isValidHandle(entityHandle))	{ return (compHandle); }
	compHandle._pool = getPool<Component>();
	uint32_t		entityIndex = Entity::getIndex(entityHandle);
	if (compHandle._pool->indices.size() <= entityIndex)
		compHandle._pool->indices.resize(entityIndex + 1, Entity::NOT_REGISTERED);
	compHandle._index = compHandle._pool->indices[entityIndex];
	if (compHandle._index != Entity::NOT_REGISTERED) {
		std::cout << "Cannot add a component when one already exists. " <<
			"Use getComponent to get it and modify to modifiy it." << std::endl;
		compHandle._comp = &compHandle._pool->components[*compHandle._index];
		return (compHandle);
	}
	Component	&component = compHandle._pool->components.emplace_back(std::forward<Args>(args)...);
	compHandle._pool->entities.push_back(entityHandle);
	compHandle._pool->indices[entityIndex] = compHandle._pool->components.size() - 1;
	compHandle._index = compHandle._pool->indices[entityIndex];
	compHandle._comp = &component;
	compHandle._pool->isDirty = true;
	prepareComponent(component);
	return (compHandle);
}

template <typename Component>
ComponentHandle<Component>	Registry::getComponent(Entity::id entityHandle) {
	ComponentHandle<Component>	compHandle;
	if (!isValidHandle(entityHandle))	{ return (compHandle); }
	compHandle._pool = getPool<Component>();
	uint32_t	entityIndex = Entity::getIndex(entityHandle);
	if (compHandle._pool->indices.size() <= entityIndex)
		return (compHandle);
	uint32_t	denseIndex = compHandle._pool->indices[entityIndex];
	if (denseIndex == Entity::NOT_REGISTERED)
		return (compHandle);
	compHandle._index = denseIndex;
	compHandle._comp = &compHandle._pool->components[denseIndex];
	return (compHandle);
}

template <typename Component>
void	Registry::removeComponent(Entity::id handle) {
	if (!isValidHandle(handle))	{ return ; }
	Pool<Component>	&pool = getPool<Component>();
	pool.removeEntity(handle);
}



template <typename Component>
Pool<Component>	*Registry::getPool() {
	std::type_index	typeKey = typeid(Component);

	auto	pool = _pools.find(typeKey);
	if (pool == _pools.end()) {
		_pools[typeKey] = std::make_unique<Pool<Component>>();
		return (static_cast<Pool<Component> *>(_pools[typeKey].get()));
	}
	return (static_cast<Pool<Component> *>(pool->second.get()));
}

template <typename... Components>
View<Components...> Registry::view() {
	static_assert(sizeof...(Components) > 0, "Cannot create an empty View. Please provide at least one component");
    static_assert(is_unique<Components...>::value, "View contains duplicate component types");
	return (View<Components...>(*this));
}

template <typename Component>
void	Registry::prepareComponent(Component &component) {
	if constexpr (requires { component.init(_assetManager); }) {
		component.init(_assetManager);
	}
}



template <typename Component>
ModificationProxy<Component>	ComponentHandle<Component>::modify(void) {
	if (_index.has_value())
		return {const_cast<Component *>(_comp), _pool, *_index};
	return {};
}

}
