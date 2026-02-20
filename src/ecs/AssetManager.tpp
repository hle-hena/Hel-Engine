/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: AssetManager.tpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 14:40:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/20 15:51:19                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/AssetManager.hpp"

namespace	hel {

template<typename T, typename = void>
struct GetPoolType { using type = T; };

template<typename T>
struct GetPoolType<T, std::void_t<typename T::AssetPool>> { using type = typename T::AssetPool; };

template <typename Component>
std::shared_ptr<Component>	AssetManager::get(const std::string &path) {
	using PoolType = GetPoolType<Component>::type;
	assetGroup	&group = _assets[typeid(Component)];
	if (group.find(path) != group.end()) {
		auto	existing = std::static_pointer_cast<Component>(group[path]);
		if constexpr (requires(Component c) { c.isLoadedFully(); }) {
			if (existing->isLoadedFully())	{ return (existing); }
		} else	{ return (existing); }
	}
	std::shared_ptr<Component>	ptr = load<Component>(path);
	if (ptr)
		group[path] = ptr;
	return (ptr);
}

template <typename Component>
std::shared_ptr<Component>	AssetManager::load(const std::string &path) {
	return (Component::load(_device, path));
}

}
