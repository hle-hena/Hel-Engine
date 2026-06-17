/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HideMouse.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:14:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/15 19:10:22                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/systems/HelSystem.hpp"

namespace	hel {

class	InputState;

}

namespace	hel::sys {

class	HideMouse : public ISystem {
	public:
		HideMouse(void) = default;
		~HideMouse(void) = default;

		void	init(void) override;

		void	update(const FrameContext &);

	private:
		InputState	*_inputState;
};

}
