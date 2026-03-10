/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EntityHierarchyUI.hpp                                               */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/28 13:55:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/10 16:22:12                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/View.hpp"
# include "ecs/Entity.hpp"
# include "api/ImGui/imgui.h"

namespace	hel {

class	Window;
class	Registry;

}

namespace	hel::sys {

class	EntityHierarchyUI {
	public:
		EntityHierarchyUI(Registry &registry) : _registry{registry} {}
		~EntityHierarchyUI(void) = default;

		void	render(Window *window, ImVec2 pos, ImVec2 size);

	private:
		void	moveEntity(Window *window, View<comp::Hierarchy> &view,
					Entity::id srcHandle, Entity::id dstHandle);
		void	showEntity(Window *window, View<comp::Hierarchy> view,
					Entity::id handle);

		Registry	&_registry;
};

}
