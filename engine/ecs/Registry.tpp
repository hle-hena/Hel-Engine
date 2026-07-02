/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 14:42:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/02 14:55:30                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "Registry.hpp"
#include "ecs/View.hpp"
#include "api/vulkan/Swapchain.hpp"
#include <iostream>
#include <tuple>

namespace	hel {

template <ValidComponent Component>
ComponentHandle<Component>	Registry::addComponent(Entity::id entityHandle) {
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
		return (compHandle);
	}
	compHandle._pool->components.emplace_back();
	compHandle._pool->compDirty.emplace_back(false);
	compHandle._pool->entities.push_back(entityHandle);
	compHandle._pool->indices[entityIndex] = static_cast<uint32_t>(compHandle._pool->components.size()) - 1;
	compHandle._index = compHandle._pool->indices[entityIndex];
	compHandle._pool->isDirty = true;
	return (compHandle);
}

template <ValidComponent... Components>
std::tuple<ComponentHandle<Components>...>	Registry::addComponents(Entity::id handle) {
	static_assert(is_unique<Components...>::value, "Duplicate values in the addComponents call.");
	return (std::make_tuple(addComponent<Components>(handle)...));
}

template <ValidComponent Component>
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
	return (compHandle);
}

template <ValidComponent Component>
void	Registry::removeComponent(Entity::id handle) {
	if (!isValidHandle(handle))	{ return ; }
	Pool<Component>	*pool = getPool<Component>();
	pool->removeEntity(handle);
}



template <ValidComponent Component>
Pool<Component>	*Registry::getPool() {
	std::type_index	typeKey = typeid(Component);

	auto	pool = _pools.find(typeKey);
	if (pool == _pools.end()) {
		_pools[typeKey] = std::make_unique<Pool<Component>>();
		return (static_cast<Pool<Component> *>(_pools[typeKey].get()));
	}
	return (static_cast<Pool<Component> *>(pool->second.get()));
}

template <ValidComponent Include, ValidComponent Exclude>
View<Include, Exclude>	Registry::view() {
	//TODO -> check if Include or Exclude has duplicates/isEmpty.
	return (View<Include, Exclude>(*this));
}

template <ValidComponent T>
constexpr bool	isGpuVisible() {
	if constexpr (requires { T::gpuVisible; })
		return T::gpuVisible;
	else
		return false;
}

template <ValidComponent... Component>
DescriptorSet::ptr	Registry::buildComponentSet(Device &device,
												DescriptorPool *dynamicPool) {
	bool	invalid = (!isGpuVisible<Component>() || ...);
	if (invalid)
		return (nullptr);
	(getPool<Component>()->flushWrites(device), ...);
	auto		factory = DescriptorFactory(device);
	uint32_t	binding = 0;
	((factory.addBinding(binding++, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		VK_SHADER_STAGE_ALL), (void)sizeof(Component)), ...);
	auto	set = factory.build(*dynamicPool);
	auto	writer = DescriptorWriter(device, set.get());
	binding = 0;
	(writer.writeBuffer(0, binding++, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		*getPool<Component>()->buffer), ...);
	writer.update();
	return (set);
}

}
