/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EntityHierarchy.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/14 19:23:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/09 18:44:03                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <ui/ImGui/imgui.h>

# include "ecs/View.hpp"
# include "ecs/Component.hpp"
# include "ecs/Entity.hpp"
# include "platform/ui/Panel.hpp"

namespace	hel::sys {

class	EntityHierarchy : public Panel<EntityHierarchy> {
	public:
		static constexpr const char	*label = "Entity";
		EntityHierarchy(void) = default;
		~EntityHierarchy(void) = default;

		expected<void, std::string>	onInit(void) override;

		void	render(Window *window, const ImVec2 &) override;

	private:
		void	moveEntity(View<include<comp::Hierarchy>> &view,
					Entity::id srcHandle, Entity::id dstHandle);
		void	showEntity(Window *window, View<include<comp::Hierarchy>> view,
					Entity::id handle);
};

}
