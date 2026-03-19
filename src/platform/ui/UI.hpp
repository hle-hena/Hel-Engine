/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/19 11:06:46                                        */
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

		template <typename T>
		void	addNewPanel(Dock *dock);

		void	registerUI(const FrameContext &ctx) override;

	private:
		void	addDock(Window *window, const ImVec2 &size);

		std::unique_ptr<Dock>	_dock;

		std::vector<std::unique_ptr<IPanel>>	_panels;
		EntityHierarchy				_entityHierarchy;
		EntityHierarchy				_entityHierarchy1;
		EntityHierarchy				_entityHierarchy2;
		EntityHierarchy				_entityHierarchy3;
		EntityHierarchy				_entityHierarchy4;
		EntityHierarchy				_entityHierarchy5;
		Inspector					_inspector;
		StyleEditor					_styleEditor;
		SceneViewport				_sceneViewport;
};

}

# include "platform/ui/UI.tpp"
