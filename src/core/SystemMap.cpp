/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemMap.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 11:00:30 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/05/29 11:39:49                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "core/SystemMap.hpp"

namespace	hel {

SystemMap::UnderlyingMap	SystemMap::_data{};

SystemMap::Iterator::Iterator(UnderlyingMap::iterator mIt,
							UnderlyingMap::iterator mEnd)
	: mapIt(mIt), mapEnd(mEnd)
{
	if (mapIt != mapEnd)
		vecIt = mapIt->second.begin();
	advanceIfEmpty();
}

SystemMap::Iterator::reference	SystemMap::Iterator::operator*(void) const
	{ return **vecIt; }
SystemMap::Iterator::pointer	SystemMap::Iterator::operator->(void) const
	{ return vecIt->get(); }

SystemMap::Iterator	&SystemMap::Iterator::operator++(void) {
	++vecIt;
	advanceIfEmpty();
	return *this;
}

SystemMap::Iterator	SystemMap::Iterator::operator++(int) {
	Iterator tmp = *this;
	++(*this);
	return tmp;
}

bool	SystemMap::Iterator::operator==(const Iterator &o) const {
	if (mapIt != o.mapIt)	{ return false; }
	if (mapIt == mapEnd)	{ return true; }
	return vecIt == o.vecIt;
}

bool	SystemMap::Iterator::operator!=(const Iterator &o) const {
	return !(*this == o);
}

void	SystemMap::Iterator::advanceIfEmpty(void) {
	while (mapIt != mapEnd && vecIt == mapIt->second.end()) {
		++mapIt;
		if (mapIt != mapEnd)
			vecIt = mapIt->second.begin();
	}
}


SystemMap::ConstIterator::ConstIterator(UnderlyingMap::const_iterator mIt,
			UnderlyingMap::const_iterator mEnd)
	: mapIt(mIt), mapEnd(mEnd)
{
	if (mapIt != mapEnd)
		vecIt = mapIt->second.begin();
	advanceIfEmpty();
}

SystemMap::ConstIterator::ConstIterator(const Iterator &it)
	: mapIt(it.mapIt), mapEnd(it.mapEnd), vecIt(it.vecIt) {}

SystemMap::ConstIterator::reference
	SystemMap::ConstIterator::operator*(void) const		{ return **vecIt; }
SystemMap::ConstIterator::pointer
	SystemMap::ConstIterator::operator->(void) const	{ return vecIt->get(); }

SystemMap::ConstIterator	&SystemMap::ConstIterator::operator++(void) {
	++vecIt;
	advanceIfEmpty();
	return *this;
}

SystemMap::ConstIterator	SystemMap::ConstIterator::operator++(int) {
	ConstIterator tmp = *this;
	++(*this);
	return tmp;
}

bool	SystemMap::ConstIterator::operator==(const ConstIterator &o) const {
	if (mapIt != o.mapIt)	return false;
	if (mapIt == mapEnd)	return true;
	return vecIt == o.vecIt;
}

bool	SystemMap::ConstIterator::operator!=(const ConstIterator &o) const {
	return !(*this == o);
}

void SystemMap::ConstIterator::advanceIfEmpty() {
	while (mapIt != mapEnd && vecIt == mapIt->second.end()) {
		++mapIt;
		if (mapIt != mapEnd)
			vecIt = mapIt->second.begin();
	}
}


SystemMap::Iterator	SystemMap::begin() {
	return Iterator(_data.begin(), _data.end());
}
SystemMap::Iterator	SystemMap::end() {
	return Iterator(_data.end(), _data.end());
}

SystemMap::ConstIterator	SystemMap::begin() const {
	return ConstIterator(_data.cbegin(), _data.cend());
}
SystemMap::ConstIterator	SystemMap::end() const {
	return ConstIterator(_data.cend(), _data.cend());
}

SystemMap::ConstIterator	SystemMap::cbegin() const {
	return begin();
}
SystemMap::ConstIterator	SystemMap::cend() const {
	return end();
}

}
