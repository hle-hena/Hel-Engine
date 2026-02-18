/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: CameraSystem.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 11:50:40 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 11:18:43                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/system/ISystem.hpp"

namespace	hel {

class	CameraSystem : public ISystem {
	public:
		CameraSystem(Device &device, Registry &registry,
			VkDescriptorSetLayout &setLayout);
		~CameraSystem(void) override;

		void	update(float deltaTime) override;

	private:
};

}
