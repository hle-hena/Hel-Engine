/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: AssetManager.tpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 14:40:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/08 17:31:58                                        */
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

template <typename Asset>
std::shared_ptr<Asset>	AssetManager::get(const std::string &path) {
	using PoolType = GetPoolType<Asset>::type;
	assetGroup	&group = _assets[typeid(PoolType)];
	if (group.find(path) != group.end()) {
		auto	basePtr = std::static_pointer_cast<PoolType>(group[path]);

		auto	existing = std::dynamic_pointer_cast<Asset>(basePtr);
		if (existing) {
			if constexpr (requires(Asset c) { c.isLoadedFully(); }) {
				if (existing->isLoadedFully())	{ return (existing); }
			} else	{ return (existing); }
		}
	}
	std::shared_ptr<Asset>	ptr = load<Asset>(path);
	if (ptr)
		group[path] = ptr;
	return (ptr);
}

template <typename Asset>
std::shared_ptr<Asset>	AssetManager::load(const std::string &path) {
	return (Asset::load(_device, path));
}

}
