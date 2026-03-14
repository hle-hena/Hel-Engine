/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 19:22:23                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "ecs/systems/ISystem.hpp"

namespace	hel::sys {

class	Transform : public ISystem {
	public:
		Transform(void) = default;
		~Transform(void) = default;

		void	init(void) override;

		void	update(const FrameContext &ctx) override;

	private:
};

}
