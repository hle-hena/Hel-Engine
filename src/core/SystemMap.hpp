/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemMap.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 10:51:42 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/05/29 15:32:22                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <memory>
#include <cstdint>
#include <vector>
#include <map>
#include <iterator>

#include "ecs/systems/ISystem.hpp"

namespace hel {

struct	SystemMap {
	using SysPtr		= std::unique_ptr<sys::ISystem>;
	using UnderlyingMap	= std::map<uint32_t, std::vector<SysPtr>>;

	struct	Iterator {
		using iterator_category	= std::forward_iterator_tag;
		using value_type		= sys::ISystem;
		using difference_type	= std::ptrdiff_t;
		using pointer			= sys::ISystem*;
		using reference			= sys::ISystem&;

		UnderlyingMap::iterator			mapIt;
		UnderlyingMap::iterator			mapEnd;
		std::vector<SysPtr>::iterator	vecIt = {};

		Iterator(UnderlyingMap::iterator mIt, UnderlyingMap::iterator mEnd);

		reference	operator*(void) const;
		pointer		operator->(void) const;

		Iterator	&operator++(void);
		Iterator	operator++(int);

		bool	operator==(const Iterator &o) const;
		bool	operator!=(const Iterator &o) const;

		private:
			void	advanceIfEmpty(void);
	};

	struct	ConstIterator {
		using iterator_category	= std::forward_iterator_tag;
		using value_type		= const sys::ISystem;
		using difference_type	= std::ptrdiff_t;
		using pointer			= const sys::ISystem*;
		using reference			= const sys::ISystem&;

		UnderlyingMap::const_iterator			mapIt;
		UnderlyingMap::const_iterator			mapEnd;
		std::vector<SysPtr>::const_iterator		vecIt = {};

		ConstIterator(UnderlyingMap::const_iterator mIt,
					UnderlyingMap::const_iterator mEnd);
		ConstIterator(const Iterator &it);

		reference	operator*(void) const;
		pointer		operator->(void) const;

		ConstIterator	&operator++(void);
		ConstIterator	operator++(int);

		bool	operator==(const ConstIterator &o) const;
		bool	operator!=(const ConstIterator &o) const;

		private:
			void advanceIfEmpty();
	};

	Iterator		begin();
	Iterator		end();

	ConstIterator	begin() const;
	ConstIterator	end() const;

	ConstIterator	cbegin() const;
	ConstIterator	cend() const;

	template <typename SysType>
	static void	addSystem(uint32_t level) {
		_data[level].push_back(std::make_unique<SysType>());
	}

	PASSKEY(EngineKey, Engine)
	static void	clear(EngineKey) {
		_data.clear();
	}

	private:
		static UnderlyingMap	_data;
};

template <typename SysType>
struct	SystemRegistrar {
	SystemRegistrar(uint32_t level = 0) {
		SystemMap::addSystem<SysType>(level);
	}
};

}
