/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EditorController.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:58:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/26 15:48:29                                        */
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

namespace	hel {

class	InputState;

}

namespace	hel::sys {

class	EditorController : public ISystem {
	public:
		EditorController(Device &device, Registry &registry);
		~EditorController(void) override;

		void	update(float deltaTime) override;

	private:
		void	handleKeyboardInput(Entity::id handle, float deltaTime);
		void	handleMouseMove(Entity::id handle);

		InputState	&_input;
};

}
