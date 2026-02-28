/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/28 12:27:15                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/systems/ISystem.hpp"
# include "ecs/Entity.hpp"
# include "ecs/View.hpp"
# include "ecs/systems/core/ui/InspectorUI.hpp"

namespace	hel {

class	AssetManager;
class	Window;

}

namespace	hel::sys {

class	UI : public ISystem {
	public:
		UI(Device &device, Registry &registry);
		~UI(void) override;

		void	render(VkRenderPass renderPass, WindowResources &resources,
			uint32_t currentFrame) override;

	private:
		void	moveEntity(Window *window, View<comp::Hierarchy> view,
						Entity::id srcHandle, Entity::id dstHandle);
		void	showEntity(Window *window, View<comp::Hierarchy> view,
						Entity::id handle);
		void	showEntitiesTab(Window *window);

		AssetManager				&_assetManager;
		InspectorUI					_inspectorUI;
};

}
