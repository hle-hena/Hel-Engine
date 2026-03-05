/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EditorUI.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 21:55:02 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/05 15:52:54                                        */
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
class	Window;

}

namespace	hel::sys {

class	InspectorUI {
	public:
		using UIDrawFunc = std::function<void(Window *, void *)>;

		InspectorUI(Registry &registry) : _registry{registry} {}
		~InspectorUI(void) = default;

		template <typename Component>
		void	setDrawFunc(UIDrawFunc func) {
			_drawFuncs[typeid(Component)] = func;
		}
		void	setBuiltInDrawFunc(void);

		void	render(Window *window);

	private:
		void	addNewComponentPopup(Entity::id handle);
		void	removeEntity(Entity::id handle);

		Registry	&_registry;
		bool		_addNewComp{false};
		int			_newCompTypeIndex{0};
		float		_windowWidth{300.f};

		std::unordered_map<std::type_index, UIDrawFunc>	_drawFuncs;
};

}
