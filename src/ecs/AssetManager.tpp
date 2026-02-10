/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: AssetManager.tpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 14:40:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/10 16:47:07                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/AssetManager.hpp"

#include <iostream>

namespace	hel {

template <typename Component>
std::shared_ptr<Component>	AssetManager::get(const std::string &path) {
	assetGroup	&group = _assets[typeid(Component)];
	if (group.find(path) != group.end())
		return (std::static_pointer_cast<Component>(group[path]));
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
