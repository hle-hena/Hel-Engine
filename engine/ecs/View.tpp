/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: View.tpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 16:09:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/02 17:24:33                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "View.hpp"

namespace	hel {

template <ValidComponent... Include, ValidComponent... Exclude>
View<include<Include...>, exclude<Exclude...>>::View(Registry &registry)
	:	_registry{registry},
		_maxEntities{0} {
	_includePools = std::make_tuple(_registry.getPool<Include>()...);
	_excludePools = std::make_tuple(_registry.getPool<Exclude>()...);
	_leadEntityList = findSmallestPool();
	if (_leadEntityList)
		_maxEntities = _leadEntityList->size();
}

template <ValidComponent... Include, ValidComponent... Exclude>
template <ValidComponent Component>
ComponentHandle<Component>	View<include<Include...>, exclude<Exclude...>>::get(Entity::id handle) const {
	ComponentHandle<Component>	compHandle;
	compHandle._pool = std::get<Pool<Component>*>(_includePools);
	compHandle._index = compHandle._pool->indices[Entity::getIndex(handle)];
	return (compHandle);
}

template <ValidComponent... Include, ValidComponent... Exclude>
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

template <ValidComponent... Include, ValidComponent... Exclude>
void	View<include<Include...>, exclude<Exclude...>>::Iterator::moveNext(void) {
	while (index < view._maxEntities) {
		Entity::id	handle = (*view._leadEntityList)[index];
		if (isValid(handle))
			break ;
		index++;
	}
}

template <ValidComponent... Include, ValidComponent... Exclude>
bool		View<include<Include...>, exclude<Exclude...>>::Iterator::isValid(Entity::id handle) {
	uint32_t	entityIndex = Entity::getIndex(handle);
	bool		hasAllIncluded = std::apply([entityIndex, handle](auto*... pools){
		return (... && (entityIndex < pools->indices.size() &&
						pools->indices[entityIndex] != IPool::UNDEFINED &&
						pools->entities[pools->indices[entityIndex]] == handle));
	}, view._includePools);
	bool		hasAnyExcluded = std::apply([entityIndex, handle](auto*... pools){
		return (... || (entityIndex < pools->indices.size() &&
						pools->indices[entityIndex] != IPool::UNDEFINED &&
						pools->entities[pools->indices[entityIndex]] == handle));
	}, view._excludePools);
	return (hasAllIncluded && !hasAnyExcluded);
}



template <ValidComponent... Include, ValidComponent... Exclude>
typename View<include<Include...>, exclude<Exclude...>>::Iterator	View<include<Include...>, exclude<Exclude...>>::begin(void) {
	Iterator	it {0, *this};
	it.moveNext();
	return (it);
}

template <ValidComponent... Include, ValidComponent... Exclude>
typename View<include<Include...>, exclude<Exclude...>>::Iterator	View<include<Include...>, exclude<Exclude...>>::end(void) {
	return {_maxEntities, *this};
}

template <ValidComponent... Include, ValidComponent... Exclude>
Entity::id	View<include<Include...>, exclude<Exclude...>>::Iterator::operator*(void) const {
	return ((*view._leadEntityList)[index]);
}

template <ValidComponent... Include, ValidComponent... Exclude>
typename View<include<Include...>, exclude<Exclude...>>::Iterator	&View<include<Include...>, exclude<Exclude...>>::Iterator::operator++(void) {
	index++;
	moveNext();
	return (*this);
}

template <ValidComponent... Include, ValidComponent... Exclude>
bool		View<include<Include...>, exclude<Exclude...>>::Iterator::operator==(const View<include<Include...>, exclude<Exclude...>>::Iterator &other) const {
	return (index == other.index);
}

template <ValidComponent... Include, ValidComponent... Exclude>
bool		View<include<Include...>, exclude<Exclude...>>::Iterator::operator!=(const View<include<Include...>, exclude<Exclude...>>::Iterator &other) const {
	return (index != other.index);
}


}
