/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EditorUI.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 21:55:02 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 22:02:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <unordered_map>
# include <typeindex>
# include <functional>

# include "ecs/Entity.hpp"

namespace	hel {

class	Registry;

}

namespace	hel::sys {

class	InspectorUI {
	public:
		using UIDrawFunc = std::function<void(void *)>;

		template <typename Component>
		void	setDrawFunc(UIDrawFunc func) {
			_drawFuncs[typeid(Component)] = func;
		}

		void	renderInspector(Registry &registry, Entity::id handle);

	private:
		std::unordered_map<std::type_index, UIDrawFunc>	_drawFuncs;
};

}
