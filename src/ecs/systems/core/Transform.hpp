/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 18:01:11                                        */
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

class	STransform : public ISystem {
	public:
		STransform(Device &device, Registry &registry,
						VkDescriptorSetLayout &setLayout);
		~STransform(void) override;

		void	update(float deltaTime) override;

	private:
};

}
