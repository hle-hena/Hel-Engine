/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 12:24:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/27 16:32:32                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <cstdint>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include <optional>

#include "ecs/Entity.hpp"
#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Descriptors.hpp"
#include "ecs/AssetManager.hpp"

namespace	hel {

template <typename... Components>
struct	include {};
template <typename... Components>
struct	exclude {};
template <typename Include, typename  Exclude = exclude<>>
class	View;

template <typename... T>
struct	is_unique;

template <>
struct	is_unique<> : std::true_type {};

template <typename T, typename... Rest>
struct	is_unique<T, Rest...> : std::bool_constant<
	(!std::is_same_v<T, Rest> && ...) && is_unique<Rest...>::value
> {};

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

template <typename Component>
struct	Pool : IPool {
	Pool(void) = default;

	std::vector<uint32_t>	indices{};
	std::vector<Entity::id>	entities{};
	std::vector<Component>	components{};

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

template <typename Component>
struct	ModificationProxy {
	ModificationProxy(void) {};
	ModificationProxy(Pool<Component> *pool, uint32_t denseIndex)
		:	_pool{pool},
			_index{denseIndex} {}
	~ModificationProxy(void) {
		auto	component = &_pool->components[*_index];
		if constexpr (requires { component->isDirty = true; }) {
			if (component)
				component->isDirty = true;
		}
		_pool->addWrite(*_index, component);
	}
	Component	*operator->(void) { return &_pool->components[*_index]; };
	explicit operator bool() const { return (_index.has_value()); }

	private:
		Pool<Component>			*_pool{nullptr};
		std::optional<uint32_t>	_index;
};

template <typename Component>
struct	ComponentHandle;

class	Registry {
	public:
		using PoolMap = std::unordered_map<std::type_index,
										std::unique_ptr<IPool>>;

		Registry(void) = default;
		~Registry(void) = default;
		Registry(const Registry &) = delete;
		Registry	&operator=(const Registry &) = delete;

		void	init(Device *device);

		AssetManager	*assetManager(void) {
			return (&_assetManager);
		}
		PoolMap			&getPools(void) {
			return (_pools);
		}

		bool	isValidHandle(Entity::id handle);

		template <typename Component>
		ComponentHandle<Component>	addComponent(Entity::id handle);
		template <typename... Components>
		std::tuple<ComponentHandle<Components>...>	addComponents(Entity::id handle);
		template <typename Component>
		ComponentHandle<Component>	getComponent(Entity::id handle);
		template <typename Component>
		void			removeComponent(Entity::id handle);

		Entity::id	createEntity(void);
		void		removeEntity(Entity::id handle);

		void		resetAllDirty(void);
		void		updateBuffers(Device &device);

		template <typename... Component>
		DescriptorSet::ptr	buildComponentSet(Device &device, DescriptorPool *dynamicPool);

		template <typename Include, typename Exclude = exclude<>>
		View<Include, Exclude> view();

	private:
		template <typename Component>
		Pool<Component>			*getPool();

		std::vector<Entity::id>		_aliveEntities{};
		PoolMap						_pools;
		AssetManager				_assetManager;

	template <typename Include, typename Exclude>
	friend class View;
};

template <typename Component>
struct	ComponentHandle {
	public:
		ComponentHandle(void) = default;

		operator bool(void) const	{ return (_index.has_value()); }
		const Component	*operator->(void)	{ return (&_pool->components[*_index]); }
		ModificationProxy<Component>	modify(void);
		uint32_t						getDenseIndex(void) const {
			return (_index.value_or(Entity::NOT_REGISTERED));
		}

	private:
		Pool<Component>			*_pool{nullptr};
		std::optional<uint32_t>	_index;
	friend class	Registry;
	template <typename Include, typename Exclude>
	friend class View;
};

}

# include "ecs/Registry.tpp"
