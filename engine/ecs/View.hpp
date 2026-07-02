/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: View.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 16:04:26 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/02 14:47:09                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vector>
# include <tuple>
# include "ecs/Registry.hpp"

namespace	hel {

template <ValidComponent... Include, ValidComponent... Exclude>
class	View<include<Include...>, exclude<Exclude...>> {
	public:
		View(Registry &registry);

		template <ValidComponent Component>
		ComponentHandle<Component>	get(Entity::id handle) const;

		struct	Iterator {
			using iterator_category = std::forward_iterator_tag;
			using value_type = Entity::id;
			using difference_type = std::ptrdiff_t;
			using pointer = Entity::id*;
			using reference = Entity::id&;

			size_t	index;
			View	&view;

			Entity::id	operator*(void) const;
			Iterator	&operator++(void);
			bool		operator==(const Iterator &other) const;
			bool		operator!=(const Iterator &other) const;

			void		moveNext(void);
			bool		isValid(Entity::id handle);
		};
		Iterator	begin(void);
		Iterator	end(void);

	private:
		std::vector<Entity::id>		*findSmallestPool(void);

		Registry							&_registry;
		std::tuple<Pool<Include>* ...>		_includePools;
		std::tuple<Pool<Exclude>* ...>		_excludePools;
		std::vector<Entity::id>				*_leadEntityList;
		size_t								_maxEntities;
};

}

#include "ecs/View.tpp"
