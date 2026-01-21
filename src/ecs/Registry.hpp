/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Registry.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 12:24:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/21 18:16:36                                        */
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

namespace	hel {

class	AssetManager {
};

using EntityId = uint32_t;

struct	IPool {
	virtual ~IPool(void) = default;
};

template <typename Component>
struct	Pool : IPool {
	std::vector<EntityId>	sparseArray{};
	std::vector<Component>	denseArray{};
};

class	Registry {
	public:
		static constexpr uint32_t NOT_REGISTERED = 0xFFFFFFFF;

		Registry(void) = default;
		~Registry(void) = default;
		Registry(const Registry &) = delete;
		Registry	&operator=(const Registry &) = delete;

		template <typename Component, typename... Args>
		Component	&addComponent(EntityId entity, Args&&... args);
		template <typename Component>
		Component	&getComponent(EntityId entity);
		template <typename Component>
		Component	*tryGetComponent(EntityId entity);
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
