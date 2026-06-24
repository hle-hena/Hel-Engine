/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ComponentManager.cpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/23 10:02:09 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/23 10:25:41                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/ComponentManager.hpp"
#include "ecs/Registry.hpp"

namespace	hel {

void	ComponentManager::addComponent(Registry &registry, Entity::id handle,
									const std::string &componentName)
{
	auto	it = _compTypes.find(componentName);
	if (it == _compTypes.end()) {
		std::cerr << "The component \"" << componentName
			<< "\" doesn't exist.\n";
		return ;
	}
	auto	type = it->second;
}

}
