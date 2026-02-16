/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ControllerSystem.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:58:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/16 15:07:12                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/system/ISystem.hpp"
# include "ecs/Entity.hpp"

namespace	hel {

class	InputState;

class	ControllerSystem : public ISystem {
	public:
		ControllerSystem(Device &device, Registry &registry,
						VkDescriptorSetLayout &setLayout);
		~ControllerSystem(void) override;

		void	update(void) override;

	private:
		void	handleKeyboardInput(Entity::id handle);
		void	handleMouseMove(Entity::id handle);

		InputState	&_input;
};

}
