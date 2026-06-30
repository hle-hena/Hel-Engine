/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ComponentManager.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/23 10:01:59 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/30 12:52:59                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "ecs/IComponent.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Registry.hpp"
#include <functional>
#include <vector>
#include <unordered_map>

namespace hel {

struct	ComponentManager {
	template <typename Comp>
	static void	registerComp(void) {
		_componentList().push_back(Comp::MetaData::label);
		_componentFactory().emplace(Comp::MetaData::label,
								[](Registry *registry, Entity::id handle){
									registry->addComponent<Comp>(handle);
								});
	}

	private:
		using factoryMap = std::unordered_map<
								std::string_view,
								std::function<void(Registry *, Entity::id)>>;
		static factoryMap	&_componentFactory(void) {
			static factoryMap	componentFactory;
			return componentFactory;
		}
		using stringVec = std::vector<std::string_view>;
		static stringVec	&_componentList(void) {
			static stringVec	componentList;
			return componentList;
		}
};

template <ValidComponent CompType>
struct	ComponentRegistrar {
	ComponentRegistrar(void) {
		ComponentManager::registerComp<CompType>();
	}
};

}
