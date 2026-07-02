/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pool.cpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/01 15:27:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/01 15:37:53                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/Pool.hpp"

namespace	hel {

void	IPool::addWrite(uint32_t index, void *data) {
	PendingWrite	write{index, data};
	auto	[it, inserted] = _writes.insert(write);

	if (!inserted) {
		_writes.erase(it);
		_writes.insert(write);
	}
}

void	IPool::markDirty(uint32_t index) {
	compDirty[index] = true;
	addWrite(index, getRaw(index));
}

bool	IPool::isDirty(uint32_t index) {
	return compDirty[index];
}

void	IPool::resetDirtyFlag(void) {
	for (auto &&value: compDirty)
		value = false;
}

}
