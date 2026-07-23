/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EntityHierarchy.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/14 19:23:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/23 10:23:09                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <ui/ImGui/imgui.h>

#include "ecs/Hierarchy.hpp"
#include "systems/ui/Panel.hpp"

namespace	hel::sys {

class	EntityHierarchy : public Panel<EntityHierarchy> {
	public:
		static constexpr const char	*label = "Entity";
		EntityHierarchy(void) = default;
		~EntityHierarchy(void) = default;

		expected<void>	onInit(void) override;

		void	render(const FrameContext &ctx, const ImVec2 &) override;

	private:
		void	moveEntity(View<include<comp::Hierarchy>> &view,
					Entity::id srcHandle, Entity::id dstHandle);
		void	showEntity(Window *window, View<include<comp::Hierarchy>> view,
					Entity::id handle);
};

}
