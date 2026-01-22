/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: TransformSystem.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 15:07:06 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/22 15:13:18                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

namespace	hel {

class	Registry;

class	TransformSystem {
	public:
		TransformSystem(Registry &registry);
		~TransformSystem(void);
		TransformSystem(const TransformSystem &) = delete;
		TransformSystem	&operator=(const TransformSystem &) = delete;

		void	update(void);

	private:
		Registry	&_registry;
};

}
