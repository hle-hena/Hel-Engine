/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SurfaceAllignement.hpp                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 18:20:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 18:29:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/systems/ISystem.hpp"

namespace	hel::sys {

class	SurfaceAllignement : public ISystem {
	public:
		SurfaceAllignement(Device &device, Registry &registry,
						VkDescriptorSetLayout &setLayout);
		~SurfaceAllignement(void) override = default;

		void	update(float deltaTime) override;

	private:
};

}
