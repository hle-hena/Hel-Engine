/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ComponentManager.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/23 10:01:59 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/23 17:51:35                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <typeindex>

#include "ecs/Entity.hpp"

namespace hel {

class	Registry;

struct	ComponentMeta {
	std::string	componentLabel;
	bool		gpuVisible;
};

struct	ComponentManager {
	public:
		static const std::vector<std::string>	&getCompNames(void) {
			return _compNames;
		}
		static void	addComponent(Registry &registry, Entity::id handle,
									const std::string &componentName);

	private:
		template <typename CompType>
		static void	registerComp(const std::string &componentName) {
			_compTypes.emplace(componentName, typeid(CompType));
			_compNames.push_back(componentName);
		}

		static std::vector<std::string>							_compNames;
		static std::unordered_map<std::string, std::type_index>	_compTypes;
	
	template <typename SysType>
	friend struct	ComponentRegistrar;
};

template <typename CompType>
struct	ComponentRegistrar {
	ComponentRegistrar(const std::string &componentName) {
		ComponentManager::registerComp<CompType>(componentName);
	}
};

}
