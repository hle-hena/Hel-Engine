/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 17:57:50                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/systems/ISystem.hpp"

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
		AssetManager				&_assetManager;
};

}
