/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HideMouse.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:14:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/21 14:19:03                                        */
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
		HideMouse(Device &device, Registry &registry,
				VkDescriptorSetLayout &setLayout);
		~HideMouse(void) override = default;

		void	update(float deltaTime) override;

	private:
		InputState	&_inputState;
};

}
