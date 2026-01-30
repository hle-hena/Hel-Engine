/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Entity.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 11:27:42 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/30 16:32:25                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <cstdint>
# include <vector>

namespace	hel {

class	Entity {
	public:
		using id = uint32_t;

		static constexpr uint32_t	INDEX_BITS = 20;
		static constexpr uint32_t	INDEX_MASK = (1 << INDEX_BITS) - 1;
		static constexpr uint32_t	VERSION_MASK = ~INDEX_MASK;
		static constexpr uint32_t	NOT_REGISTERED = 0xFFFFFFFF;

		static uint32_t	getIndex(Entity::id handle) {
			return (handle & INDEX_MASK);
		}

	private:
		~Entity(void) = delete;

		static Entity::id	acquire(void) {
			if (!_freeId.empty()) {
				id	returnId = _freeId.back();
				_freeId.pop_back();
				return (returnId);
			}
			return (_maxIdGiven++ & INDEX_MASK);
		}

		static void	release(Entity::id handle) {
			uint32_t	id = handle & INDEX_MASK;
			uint32_t	version = (handle & VERSION_MASK) >> INDEX_BITS;

			version = (version + 1) & ((1 << (32 - INDEX_BITS)) - 1);
			_freeId.push_back(id | (version << INDEX_BITS));
		}

		static inline id				_maxIdGiven = 0;
		static inline std::vector<id>	_freeId{};

	friend class	Registry;
};

}
