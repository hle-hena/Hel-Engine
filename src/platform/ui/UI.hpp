/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 19:57:09                                        */
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
class	ImagePool;

}

namespace	hel::sys {

class	UI : public ISystem {
	public:
		UI(void) = default;
		~UI(void) = default;

		void	init(void) override;

		void	registerUI(const FrameContext &ctx) override;

	private:
		void	addSplitters(float windowWidth, float windowHeight);

		float	_leftTabWidth{300.f};
		float	_rightTabWidth{300.f};

		InspectorUI					_inspectorUI;
		EntityHierarchyUI			_entityHierarchyUI;
		SceneViewport				_sceneViewport;
};

}
