/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: View.tpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 16:09:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/30 20:54:25                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "View.hpp"

namespace	hel {

template <typename... Include, typename... Exclude>
View<include<Include...>, exclude<Exclude...>>::View(Registry &registry)
	:	_registry{registry},
		_maxEntities{0} {
	_includePools = std::make_tuple(_registry.getPool<Include>()...);
	_excludePools = std::make_tuple(_registry.getPool<Exclude>()...);
	_leadEntityList = findSmallestPool();
	if (_leadEntityList)
		_maxEntities = _leadEntityList->size();
}

template <typename... Include, typename... Exclude>
template <typename Component>
ComponentHandle<Component>	View<include<Include...>, exclude<Exclude...>>::get(Entity::id handle) const {
	ComponentHandle<Component>	compHandle;
	compHandle._pool = std::get<Pool<Component>*>(_includePools);
	compHandle._index = compHandle._pool->indices[Entity::getIndex(handle)];
	return (compHandle);
}

template <typename... Include, typename... Exclude>
std::vector<Entity::id>	*View<include<Include...>, exclude<Exclude...>>::findSmallestPool(void) {
	std::vector<Entity::id>	*smallestPool = nullptr;
	size_t					minSize = static_cast<size_t>(-1);

	std::apply([&](auto*... pools){
		auto	check = [&](auto *pool){
			if (pool->entities.size() < minSize) {
				minSize = pool->entities.size();
				smallestPool = &pool->entities;
			}
		};
		(check(pools), ...);
	}, _includePools);
	return (smallestPool);
}

template <typename... Include, typename... Exclude>
void	View<include<Include...>, exclude<Exclude...>>::Iterator::moveNext(void) {
	while (index < view._maxEntities) {
		Entity::id	handle = (*view._leadEntityList)[index];
		if (isValid(handle))
			break ;
		index++;
	}
}

template <typename... Include, typename... Exclude>
bool		View<include<Include...>, exclude<Exclude...>>::Iterator::isValid(Entity::id handle) {
	uint32_t	entityIndex = Entity::getIndex(handle);
	bool		hasAllIncluded = std::apply([entityIndex, handle](auto*... pools){
		return (... && (entityIndex < pools->indices.size() &&
						pools->indices[entityIndex] != Entity::NOT_REGISTERED &&
						pools->entities[pools->indices[entityIndex]] == handle));
	}, view._includePools);
	bool		hasAnyExcluded = std::apply([entityIndex, handle](auto*... pools){
		return (... || (entityIndex < pools->indices.size() &&
						pools->indices[entityIndex] != Entity::NOT_REGISTERED &&
						pools->entities[pools->indices[entityIndex]] == handle));
	}, view._excludePools);
	return (hasAllIncluded && !hasAnyExcluded);
}



template <typename... Include, typename... Exclude>
typename View<include<Include...>, exclude<Exclude...>>::Iterator	View<include<Include...>, exclude<Exclude...>>::begin(void) {
	Iterator	it {0, *this};
	it.moveNext();
	return (it);
}

template <typename... Include, typename... Exclude>
typename View<include<Include...>, exclude<Exclude...>>::Iterator	View<include<Include...>, exclude<Exclude...>>::end(void) {
	return {_maxEntities, *this};
}

template <typename... Include, typename... Exclude>
Entity::id	View<include<Include...>, exclude<Exclude...>>::Iterator::operator*(void) const {
	return ((*view._leadEntityList)[index]);
}

template <typename... Include, typename... Exclude>
typename View<include<Include...>, exclude<Exclude...>>::Iterator	&View<include<Include...>, exclude<Exclude...>>::Iterator::operator++(void) {
	index++;
	moveNext();
	return (*this);
}

template <typename... Include, typename... Exclude>
bool		View<include<Include...>, exclude<Exclude...>>::Iterator::operator==(const View<include<Include...>, exclude<Exclude...>>::Iterator &other) const {
	return (index == other.index);
}

template <typename... Include, typename... Exclude>
bool		View<include<Include...>, exclude<Exclude...>>::Iterator::operator!=(const View<include<Include...>, exclude<Exclude...>>::Iterator &other) const {
	return (index != other.index);
}


}
