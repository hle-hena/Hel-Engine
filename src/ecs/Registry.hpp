/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 12:24:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/23 18:51:48                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <cstdint>
# include <typeindex>
# include <typeinfo>
# include <unordered_map>
# include <vector>
# include <memory>

namespace	hel {

class	AssetManager {
	//TODO -> Implement an actual asset manager, but probably in another file.
};

template <typename... Components>
class View;

using EntityId = uint32_t;
static constexpr uint32_t NOT_REGISTERED = 0xFFFFFFFF;

template <typename... T>
struct	is_unique;

template <>
struct	is_unique<> : std::true_type {};

template <typename T, typename... Rest>
struct	is_unique<T, Rest...> : std::bool_constant<
	(!std::is_same_v<T, Rest> && ...) && is_unique<Rest...>::value
> {};

struct	IPool {
	virtual ~IPool(void) = default;
	virtual void	tryRemoveEntity(EntityId entity) = 0;
	virtual void	removeEntity(EntityId entity) = 0;
};

template <typename Component>
struct	Pool : IPool {
	std::vector<uint32_t>	indices{};
	std::vector<EntityId>	entities{};
	std::vector<Component>	components{};

	void	tryRemoveEntity(EntityId entity) override;
	void	removeEntity(EntityId entity) override;
};

class	Registry {
	public:
		Registry(void) = default;
		~Registry(void) = default;
		Registry(const Registry &) = delete;
		Registry	&operator=(const Registry &) = delete;

		template <typename Component, typename... Args>
		const Component	&addComponent(EntityId entity, Args&&... args);
		template <typename Component, typename... Args>
		const Component	&tryAddComponent(EntityId entity, Args&&... args);
		template <typename Component>
		const Component	&getComponent(EntityId entity);
		template <typename Component>
		const Component	*tryGetComponent(EntityId entity);

		template <typename Component, typename Func>
		void		patch(EntityId entity, Func&& func);
		template <typename Component, typename Func>
		void		patch(const Component &comp, Func&& func);
		template <typename Component, typename Func>
		void		update(EntityId entity, Func&& func);
		template <typename Component, typename Func>
		void		update(const Component &comp, Func&& func);

		template <typename Component>
		void		removeComponent(EntityId entity);
		template <typename Component>
		void		tryRemoveComponent(EntityId entity);
		void		removeEntity(EntityId entity);

		template <typename... Components>
		View<Components...>		view();

	private:
		template<typename Component>
		void	prepareComponent(Component &component);
		template <typename Component>
		Pool<Component>			&getPool();

		std::unordered_map<std::type_index, std::unique_ptr<IPool>>	_pools;
		AssetManager							_assetManager;

	template <typename... Components>
	friend class View;
};

}

# include "ecs/View.hpp"
# include "ecs/Registry.tpp"
