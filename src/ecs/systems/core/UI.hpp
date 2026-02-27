/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 19:34:08                                        */
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

namespace	hel {

class	AssetManager;

}

namespace	hel::sys {

class	UI : public ISystem {
	public:
		UI(Device &device, Registry &registry);
		~UI(void) override;

		void	render(VkRenderPass renderPass, WindowResources &resources,
			uint32_t currentFrame) override;

	private:
		void	moveEntity(View<comp::Hierarchy> view, Entity::id srcHandle,
						Entity::id dstHandle);
		void	showEntity(Entity::id handle, View<comp::Hierarchy> view);
		void	showEntitiesTab(void);

		AssetManager				&_assetManager;
};

}
