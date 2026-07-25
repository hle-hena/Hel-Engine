/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EditorController.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:58:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/25 17:31:55                                        */
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

class	EditorController : public ISystem {
	public:
		EditorController(void) = default;
		~EditorController(void) = default;

		void	init(void) override;

		void	handleInput(const ExecutionContext &ctx);

	private:
		void	handleKeyboardInput(Entity::id handle, float deltaTime);
		void	handleMouseMove(Entity::id handle);
};

}
