/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 12:24:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 20:48:44                                        */
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
# include "platform/input/InputState.hpp"

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
	virtual void	removeEntity(Entity::id handle) = 0;
	virtual void	resetDirtyFlag(void) = 0;
};

template <typename Component>
struct	Pool : IPool {
	std::vector<uint32_t>	indices{};
	std::vector<Entity::id>	entities{};
	std::vector<Component>	components{};

	void	removeEntity(Entity::id handle) override;
	void	resetDirtyFlag(void) override;
};

template <typename Component>
struct	ModificationProxy {
	Component	*component;
	ModificationProxy(Component *comp) : component(comp) {}
	~ModificationProxy(void) {
		if constexpr (requires { component->isDirty = true; })
			component->isDirty = true;
	}
	Component	*operator->(void) { return component; };
	explicit operator bool() const { return (component != nullptr); }
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

		InputState		&getInputState(void) {
			return (_inputState);
		}

		template <typename Component, typename... Args>
		const Component	*addComponent(Entity::id handle, Args&&... args);
		template <typename Component>
		const Component	*getComponent(Entity::id handle);
		template <typename Component>
		void			removeComponent(Entity::id handle);

		template <typename Component>
		ModificationProxy<Component>	modify(Entity::id handle);
		template <typename Component>
		ModificationProxy<Component>	modify(const Component *component);

		Entity::id	createEntity(void);
		void		removeEntity(Entity::id handle);

		void		resetAllDirty(void);

		template <typename... Components>
		View<Components...>		view();

	private:
		template<typename Component>
		void	prepareComponent(Component &component);
		template <typename Component>
		Pool<Component>			&getPool();

		bool	isValidHandle(Entity::id handle);

		std::vector<Entity::id>										_aliveEntities{};
		std::unordered_map<std::type_index, std::unique_ptr<IPool>>	_pools;
		AssetManager												&_assetManager;
		InputState													_inputState;

	template <typename... Components>
	friend class View;
};

}

# include "ecs/View.hpp"
# include "ecs/Registry.tpp"
