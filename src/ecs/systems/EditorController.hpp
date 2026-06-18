/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EditorController.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:58:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/15 19:10:20                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/systems/HelSystem.hpp"
# include "ecs/Entity.hpp"

namespace	hel {

class	InputState;

}

namespace	hel::sys {

class	EditorController : public ISystem {
	public:
		EditorController(void) = default;
		~EditorController(void) = default;

		void	init(void) override;

		void	handleInput(const FrameContext &ctx);

	private:
		void	handleKeyboardInput(Entity::id handle, float deltaTime);
		void	handleMouseMove(Entity::id handle);

		InputState	*_input;
};

}
