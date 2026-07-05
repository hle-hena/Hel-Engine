/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 12:24:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 19:21:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>

#include "ecs/Entity.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Pool.hpp"

namespace	hel {

struct	DescriptorSet;
class	DescriptorPool;

template <ValidComponent... Components>
struct	include {};
template <ValidComponent... Components>
struct	exclude {};
template <typename Include, typename Exclude = exclude<>>
class	View;

template <typename... T>
struct	is_unique;

template <>
struct	is_unique<> : std::true_type {};

template <typename T, typename... Rest>
struct	is_unique<T, Rest...> : std::bool_constant<
	(!std::is_same_v<T, Rest> && ...) && is_unique<Rest...>::value
> {};

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

		template <ValidComponent Component>
		ComponentHandle<Component>	addComponent(Entity::id handle);
		template <ValidComponent... Components>
		std::tuple<ComponentHandle<Components>...>	addComponents(Entity::id handle);
		template <ValidComponent Component>
		ComponentHandle<Component>	getComponent(Entity::id handle);
		template <ValidComponent Component>
		void						removeComponent(Entity::id handle);

		Entity::id	createEntity(void);
		void		removeEntity(Entity::id handle);

		void		resetAllDirty(void);
		void		updateBuffers(Device &device);

		template <ValidComponent... Component>
		std::unique_ptr<DescriptorSet>	buildComponentSet(Device &device,
												DescriptorPool *dynamicPool);

		template <typename Include, typename Exclude = exclude<>>
		View<Include, Exclude> view();

	private:
		template <ValidComponent Component>
		Pool<Component>			*getPool();

		std::vector<Entity::id>		_aliveEntities{};
		PoolMap						_pools;
		AssetManager				_assetManager;

	template <typename Include, typename Exclude>
	friend class View;
};

}

# include "ecs/Registry.tpp"
