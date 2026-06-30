/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pool.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 15:55:18 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/30 16:54:04                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "ecs/IComponent.hpp"
#include "ecs/Entity.hpp"
#include <vector>
#include <cstdint>
#include <memory>
#include <set>

namespace	hel {

class	Buffer;
class	Device;

struct	PendingWrite {
	uint32_t	index;
	void		*data;

	bool	operator<(const PendingWrite &other) const {
		return (index < other.index);
	}
};

struct	IPool {
	bool	isDirty{false};

	virtual ~IPool(void) = default;
	virtual void	syncBuffer(Device &device) = 0;
	virtual void	syncBuffer(Device &device, const PendingWrite &write) = 0;
	virtual void	removeEntity(Entity::id handle) = 0;
	virtual void	resetDirtyFlag(void) = 0;
	virtual void	addWrite(uint32_t index, void *data) = 0;
	virtual void	flushWrites(Device &device) = 0;
	virtual void	removePendingBuffers(void) = 0;

	virtual bool		has(Entity::id handle) const = 0;
	virtual void		*getRaw(Entity::id handle) = 0;
	virtual const char	*getTypeName(void) const = 0;
};

template <ValidComponent Component>
struct	Pool: IPool {
	Pool(void) = default;

	std::vector<uint32_t>					indices{};
	std::vector<Entity::id>					entities{};
	std::vector<typename Component::POD>	components{};

	std::unique_ptr<Buffer>	buffer{nullptr};

	void	syncBuffer(Device &device) override;
	void	syncBuffer(Device &device, const PendingWrite &write) override;
	void	removeEntity(Entity::id handle) override;
	void	resetDirtyFlag(void) override;
	void	addWrite(uint32_t index, void *data) override;
	void	flushWrites(Device &device) override;
	void	removePendingBuffers(void) override;

	bool		has(Entity::id handle) const override;
	void		*getRaw(Entity::id handle) override;
	const char	*getTypeName(void) const override;

	private:
		std::set<PendingWrite>	_writes{};
		std::vector<std::pair<uint32_t, std::unique_ptr<Buffer>>>	_pendingBuffers;
};

}

#include "Pool.tpp"
