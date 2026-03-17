/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/17 16:37:16                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <memory>

# include "ecs/systems/ISystem.hpp"

# include "platform/ui/Dock.hpp"

# include "platform/ui/Inspector.hpp"
# include "platform/ui/EntityHierarchy.hpp"
# include "platform/ui/StyleEditor.hpp"
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
		void	addDockSpaces(Window *window, float windowWidth, float windowHeight);
		void 	initDockLayout(void);

		float	_leftTabWidth{300.f};
		float	_rightTabWidth{300.f};
		std::unique_ptr<Dock>	_leftDock;
		std::unique_ptr<Dock>	_rightDock;

		EntityHierarchy				_entityHierarchy;
		Inspector					_inspector;
		StyleEditor					_styleEditor;
		SceneViewport				_sceneViewport;
};

}
