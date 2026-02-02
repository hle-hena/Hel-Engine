/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: View.tpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 16:09:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 11:38:30                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "View.hpp"

namespace	hel {

template <typename... Components>
View<Components...>::View(Registry &registry)
	:	_registry{registry},
		_maxEntities{0} {
	_pools = std::make_tuple(&_registry.getPool<Components>()...);
	_leadEntityList = findSmallestPool();
	if (_leadEntityList)
		_maxEntities = _leadEntityList->size();
}

template <typename... Components>
template <typename Component>
const Component	*View<Components...>::get(Entity::id handle) const {
	auto	*pool = std::get<Pool<Component>*>(_pools);
	return (&pool->components[pool->indices[Entity::getIndex(handle)]]);
}

template <typename... Components>
std::vector<Entity::id>	*View<Components...>::findSmallestPool(void) {
	std::vector<Entity::id>	*smallestPool = nullptr;
	size_t					minSize = -1;

	std::apply([&](auto*... pools){
		auto	check = [&](auto *pool){
			if (pool->entities.size() < minSize) {
				minSize = pool->entities.size();
				smallestPool = &pool->entities;
			}
		};
		(check(pools), ...);
	}, _pools);
	return (smallestPool);
}

template <typename... Components>
void	View<Components...>::Iterator::moveNext(void) {
	while (index < view._maxEntities) {
		Entity::id	handle = (*view._leadEntityList)[index];
		if (isValid(handle))
			break ;
		index++;
	}
}

template <typename... Components>
bool		View<Components...>::Iterator::isValid(Entity::id handle) {
	uint32_t	entityIndex = Entity::getIndex(handle);
	return (std::apply([entityIndex, handle](auto*... pools){
		return (... && (entityIndex < pools->indices.size() &&
						pools->indices[entityIndex] != Entity::NOT_REGISTERED &&
						pools->entities[pools->indices[entityIndex]] == handle));
	}, view._pools));
}



template <typename... Components>
typename View<Components...>::Iterator	View<Components...>::begin(void) {
	Iterator	it {0, *this};
	it.moveNext();
	return (it);
}

template <typename... Components>
typename View<Components...>::Iterator	View<Components...>::end(void) {
	return {_maxEntities, *this};
}

template <typename... Components>
Entity::id	View<Components...>::Iterator::operator*(void) const {
	return ((*view._leadEntityList)[index]);
}

template <typename... Components>
typename View<Components...>::Iterator	&View<Components...>::Iterator::operator++(void) {
	index++;
	moveNext();
	return (*this);
}

template <typename... Components>
bool		View<Components...>::Iterator::operator==(const Iterator &other) const {
	return (index == other.index);
}

template <typename... Components>
bool		View<Components...>::Iterator::operator!=(const Iterator &other) const {
	return (index != other.index);
}


}
