/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HideMouse.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:14:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 18:34:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/systems/ISystem.hpp"

namespace	hel {

class	InputState;

}

namespace	hel::sys {

class	HideMouse : public ISystem {
	public:
		HideMouse(void) = default;
		~HideMouse(void) = default;

		void	init(void) override;

		void	update(const FrameContext &) override;

	private:
		InputState	*_inputState;
};

}
