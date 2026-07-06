/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HideMouse.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/21 14:14:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/06 10:54:54                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "HelSystem.hpp"

namespace	hel::sys {

class	HideMouse : public ISystem {
	public:
		HideMouse(void) = default;
		~HideMouse(void) = default;

		void	init(void) override;

		void	update(const FrameContext &);
};

}
