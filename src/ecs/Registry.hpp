/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 12:24:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/22 15:38:47                                        */
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
};

using EntityId = uint32_t;
static constexpr uint32_t NOT_REGISTERED = 0xFFFFFFFF;

struct	IPool {
	virtual ~IPool(void) = default;
	virtual void	tryRemoveEntity(EntityId entity) = 0;
	virtual void	removeEntity(EntityId entity) = 0;
};

template <typename Component>
struct	Pool : IPool {
	std::vector<uint32_t>	entityToIndex{};
	std::vector<EntityId>	indexToEntity{};
	std::vector<Component>	components{};

	void	removeEntity(EntityId entity) override;
	void	tryRemoveEntity(EntityId entity) override;
};

class	Registry {
	public:
		Registry(void) = default;
		~Registry(void) = default;
		Registry(const Registry &) = delete;
		Registry	&operator=(const Registry &) = delete;

		template <typename Component, typename... Args>
		Component	&addComponent(EntityId entity, Args&&... args);
		template <typename Component, typename... Args>
		Component	&tryAddComponent(EntityId entity, Args&&... args);
		template <typename Component>
		Component	&getComponent(EntityId entity);
		template <typename Component>
		Component	*tryGetComponent(EntityId entity);
		template <typename Component, typename Func>
		void		patch(EntityId entity, Func&& func);
		template <typename Component, typename Func>
		void		patch(Component &comp, Func&& func);

		template <typename Component>
		void		removeComponent(EntityId entity);
		template <typename Component>
		void		tryRemoveComponent(EntityId entity);
		void		removeEntity(EntityId entity);

		template <typename Component>
		Pool<Component>	&getPool();

	private:
		template<typename Component>
		void	prepareComponent(Component &component);

		std::unordered_map<std::type_index, std::unique_ptr<IPool>>	_pools;
		AssetManager							_assetManager;
};

}

# include "ecs/Registry.tpp"
