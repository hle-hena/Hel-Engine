/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pool.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 15:55:18 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/02 15:17:16                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "ecs/ValidComponent.hpp"
#include "ecs/Entity.hpp"
#include <vector>
#include <cstdint>
#include <memory>
#include <set>

namespace	hel {

class	Buffer;
class	Device;
struct	OpaqueComponentHandle;

struct	PendingWrite {
	uint32_t	index;
	void		*data;

	bool	operator<(const PendingWrite &other) const {
		return (index < other.index);
	}
};

struct	IPool {

	virtual ~IPool(void) = default;
	virtual void	syncBuffer(Device &device) = 0;
	virtual void	syncBuffer(Device &device, const PendingWrite &write) = 0;
	virtual void	flushWrites(Device &device) = 0;
	virtual void	removePendingBuffers(void) = 0;

	virtual bool	has(Entity::id handle) const = 0;
	virtual void	removeEntity(Entity::id handle) = 0;
	virtual OpaqueComponentHandle	get(Entity::id handle) = 0;

	protected:
		virtual void				*getRaw(uint32_t index) = 0;
		virtual std::string_view	getTypeName(void) const = 0;

		void				markDirty(uint32_t index);
		bool				isDirty(uint32_t index);
		void				resetDirtyFlag(void);
		std::vector<bool>			compDirty{};

		void	addWrite(uint32_t index, void *data);
		bool							GPUBufferDirty{false};
		std::set<PendingWrite>			_writes{};
		std::vector<std::pair<uint32_t,
			std::unique_ptr<Buffer>>>	_pendingBuffers;

	friend struct OpaqueComponentHandle;
	template <ValidComponent Comp>
	friend struct ComponentHandle;
	friend class Registry;
};

template <ValidComponent Component>
struct	Pool: IPool {
	Pool(void) = default;

	static constexpr uint32_t UNDEFINED = 0xFFFFFFFF;

	std::vector<uint32_t>					indices{};
	std::vector<Entity::id>					entities{};
	std::vector<typename Component::POD>	components{};

	std::unique_ptr<Buffer>	buffer{nullptr};

	void	syncBuffer(Device &device) override;
	void	syncBuffer(Device &device, const PendingWrite &write) override;
	void	flushWrites(Device &device) override;
	void	removePendingBuffers(void) override;
	
	bool	has(Entity::id handle) const override;
	void	removeEntity(Entity::id handle) override;
	OpaqueComponentHandle	get(Entity::id handle) override;

	protected:
		void				*getRaw(Entity::id handle) override;
		std::string_view	getTypeName(void) const override;
};

}

#include "Pool.tpp"
