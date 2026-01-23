/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: View.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 16:04:26 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/23 18:39:53                                        */
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

template <typename... Components>
class	View {
	public:
		View(Registry &registry);

		template <typename Component>
		const Component	&get(EntityId entity) const;

		struct	Iterator {
			using iterator_category = std::forward_iterator_tag;
			using value_type = EntityId;
			using difference_type = std::ptrdiff_t;
			using pointer = EntityId*;
			using reference = EntityId&;

			size_t	index;
			View	&view;

			EntityId	operator*(void) const;
			Iterator	&operator++(void);
			bool		operator==(const Iterator &other) const;
			bool		operator!=(const Iterator &other) const;

			void		moveNext(void);
			bool		isValid(EntityId entity);
		};
		Iterator	begin(void);
		Iterator	end(void);

	private:
		std::vector<EntityId>		*findSmallestPool(void);

		Registry							&_registry;
		std::tuple<Pool<Components>* ...>	_pools;
		std::vector<EntityId>				*_leadEntityList;
		size_t								_maxEntities;
};

}

#include "ecs/View.tpp"
