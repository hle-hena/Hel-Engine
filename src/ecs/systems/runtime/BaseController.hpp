/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: BaseController.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 18:14:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 18:33:49                                        */
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
		BaseController(Device &device, Registry &registry,
						VkDescriptorSetLayout &setLayout);
		~BaseController(void) override;

		void	update(float deltaTime) override;

	private:
		void	handleKeyboardInput(Entity::id handle, float deltaTime);
		void	handleMouseMove(Entity::id handle);

		InputState	&_input;
};

}
