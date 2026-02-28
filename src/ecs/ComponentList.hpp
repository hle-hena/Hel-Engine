/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ComponentList.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/28 16:53:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/28 17:33:30                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vector>
# include <string>

# include "ecs/Entity.hpp"
# include "ecs/Registry.hpp"
# include "ecs/Component.hpp"

namespace	hel {

class	ComponentList {
	public:
		const void	*addComponent(Registry &registry, Entity::id handle,
							const char *componentName);

		static std::vector<const char *>	&getComponentList(void) {
			return (_componentList);
		}

	private:
		static std::vector<const char *>	_componentList;
};

}
