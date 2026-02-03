/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: CameraSystem.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 11:50:40 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/03 19:57:12                                        */
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

class	CameraSystem {
	public:
		CameraSystem(Registry &registry);
		~CameraSystem(void);
		CameraSystem(const CameraSystem &) = delete;
		CameraSystem	&operator=(const CameraSystem &) = delete;

		void	update(void);

	private:
		Registry	&_registry;
};

}
