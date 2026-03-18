/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/18 10:44:58                                        */
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

}

namespace	hel::sys {

class	UI : public ISystem {
	public:
		UI(void) = default;
		~UI(void) = default;

		void	init(void) override;

		void	registerUI(const FrameContext &ctx) override;

	private:
		void	addDock(Window *window, const ImVec2 &size);

		std::unique_ptr<Dock>	_dock;

		EntityHierarchy				_entityHierarchy;
		Inspector					_inspector;
		StyleEditor					_styleEditor;
		SceneViewport				_sceneViewport;
};

}
