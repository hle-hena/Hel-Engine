/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ControllerSystem.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:58:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/03 19:11:07                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/Entity.hpp"

namespace	hel {

class	Registry;
class	InputState;

class	ControllerSystem {
	public:
		ControllerSystem(Registry &registry);
		~ControllerSystem(void);

		void	update(void);

	private:
		void	handleKeyboardInput(Entity::id handle);
		void	handleMouseMove(Entity::id handle);

		Registry	&_registry;
		InputState	&_input;
};

}
