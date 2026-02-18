/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: TransformSystem.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 15:07:06 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 11:18:46                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "ecs/system/ISystem.hpp"

namespace	hel {

class	TransformSystem : public ISystem {
	public:
		TransformSystem(Device &device, Registry &registry,
						VkDescriptorSetLayout &setLayout);
		~TransformSystem(void) override;

		void	update(float deltaTime) override;

	private:
};

}
