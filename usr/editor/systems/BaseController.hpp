/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: BaseController.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 18:14:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 15:31:24                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "core/HelSystem.hpp"

namespace	hel::sys {

class	BaseController : public ISystem {
	public:
		BaseController(void) = default;
		~BaseController(void) = default;

		void	init(void) override;

		void	handleInput(const FrameContext &ctx);

	private:
		void	handleKeyboardInput(Entity::id handle, float deltaTime);
		void	handleMouseMove(Entity::id handle);
};

}
