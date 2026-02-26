/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/26 15:49:12                                        */
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
		Transform(Device &device, Registry &registry);
		~Transform(void) override;

		void	update(float deltaTime) override;

	private:
};

}
