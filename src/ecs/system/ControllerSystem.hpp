/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ControllerSystem.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:58:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 11:19:45                                        */
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

		void	update(float deltaTime) override;

	private:
		void	handleKeyboardInput(Entity::id handle, float deltaTime);
		void	handleMouseMove(Entity::id handle);

		InputState	&_input;
};

}
