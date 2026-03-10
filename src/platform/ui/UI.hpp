/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/10 16:57:05                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/systems/ISystem.hpp"

# include "platform/ui/InspectorUI.hpp"
# include "platform/ui/EntityHierarchyUI.hpp"
# include "platform/ui/SceneViewport.hpp"

namespace	hel {

class	Window;

}

namespace	hel::sys {

class	UI : public ISystem {
	public:
		UI(Device &device, Registry &registry);
		~UI(void) override;

		void	render(const RenderingConfig &conf, WindowResources &resources,
			uint32_t currentFrame) override;

	private:
		void	addSplitters(float windowWidth, float windowHeight);

		float	_leftTabWidth{300.f};
		float	_rightTabWidth{300.f};

		InspectorUI					_inspectorUI;
		EntityHierarchyUI			_entityHierarchyUI;
		SceneViewport				_sceneViewport;
};

}
