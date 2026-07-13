/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pool.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 15:55:18 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/13 12:54:47                                        */
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
#include "api/vulkan/Buffer.hpp"

#include <vector>
#include <cstdint>
#include <set>

namespace	hel {

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
	static constexpr uint32_t UNDEFINED = 0xFFFFFFFF;

	virtual ~IPool(void) = default;
	virtual expected<void>	syncBuffer(void) = 0;
	virtual expected<void>	syncBuffer(const PendingWrite &write) = 0;
	virtual expected<void>	flushWrites(Device &device) = 0;
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

	friend struct OpaqueComponentHandle;
	template <ValidComponent Comp>
	friend struct ComponentHandle;
	friend class Registry;
};

template <ValidComponent Component>
struct	Pool: IPool {
	Pool(void) = default;

	std::vector<uint32_t>					indices{};
	std::vector<Entity::id>					entities{};
	std::vector<typename Component::POD>	components{};

	Ref<Buffer<typename Component::POD>>	buffer{nullptr};

	expected<void>	syncBuffer(void) override;
	expected<void>	syncBuffer(const PendingWrite &write) override;
	expected<void>	flushWrites(Device &device) override;
	void	removePendingBuffers(void) override;
	
	bool	has(Entity::id handle) const override;
	void	removeEntity(Entity::id handle) override;
	OpaqueComponentHandle	get(Entity::id handle) override;

	protected:
		void				*getRaw(Entity::id handle) override;
		std::string_view	getTypeName(void) const override;

		std::vector<std::pair<uint32_t, Ref<
			Buffer<typename Component::POD>>>>	_pendingBuffers;
};

}

#include "Pool.tpp"
