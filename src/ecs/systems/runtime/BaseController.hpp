/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: BaseController.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 18:14:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 19:23:37                                        */
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

class	BaseController : public ISystem {
	public:
		BaseController(void) = default;
		~BaseController(void) = default;

		void	init(void) override;

		void	update(const FrameContext &ctx) override;

	private:
		void	handleKeyboardInput(Entity::id handle, float deltaTime);
		void	handleMouseMove(Entity::id handle);

		InputState	*_input;
};

}
