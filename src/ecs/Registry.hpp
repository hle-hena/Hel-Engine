/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 12:24:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/30 15:49:37                                        */
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

# include "ecs/Entity.hpp"

namespace	hel {

class	AssetManager;
template <typename... Components>
class	View;

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
	virtual void	tryRemoveEntity(Entity::id handle) = 0;
	virtual void	removeEntity(Entity::id handle) = 0;
};

template <typename Component>
struct	Pool : IPool {
	std::vector<uint32_t>	indices{};
	std::vector<Entity::id>	entities{};
	std::vector<Component>	components{};

	void	tryRemoveEntity(Entity::id handle) override;
	void	removeEntity(Entity::id handle) override;
};

class	Registry {
	public:
		Registry(AssetManager &assetManager);
		~Registry(void) = default;
		Registry(const Registry &) = delete;
		Registry	&operator=(const Registry &) = delete;

		AssetManager	&getAssetManager(void) const {
			return (_assetManager);
		}

		template <typename Component, typename... Args>
		const Component	&addComponent(Entity::id handle, Args&&... args);
		template <typename Component, typename... Args>
		const Component	&tryAddComponent(Entity::id handle, Args&&... args);
		template <typename Component>
		const Component	&getComponent(Entity::id handle);
		template <typename Component>
		const Component	*tryGetComponent(Entity::id handle);

		template <typename Component, typename Func>
		void		patch(Entity::id handle, Func&& func);
		template <typename Component, typename Func>
		void		patch(const Component &comp, Func&& func);
		template <typename Component, typename Func>
		void		update(Entity::id handle, Func&& func);
		template <typename Component, typename Func>
		void		update(const Component &comp, Func&& func);

		template <typename Component>
		void		removeComponent(Entity::id handle);
		template <typename Component>
		void		tryRemoveComponent(Entity::id handle);

		Entity::id	createEntity(void);
		void		removeEntity(Entity::id handle);

		template <typename... Components>
		View<Components...>		view();

	private:
		template<typename Component>
		void	prepareComponent(Component &component);
		template <typename Component>
		Pool<Component>			&getPool();

		std::unordered_map<std::type_index, std::unique_ptr<IPool>>	_pools;
		AssetManager												&_assetManager;

	template <typename... Components>
	friend class View;
};

}

# include "ecs/View.hpp"
# include "ecs/Registry.tpp"
