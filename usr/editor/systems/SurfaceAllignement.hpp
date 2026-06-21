/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SurfaceAllignement.hpp                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 18:20:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/21 12:23:49                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "HelSystem.hpp"

namespace	hel::sys {

class	SurfaceAllignement : public ISystem {
	public:
		SurfaceAllignement(void) = default;
		~SurfaceAllignement(void) = default;

		void	init(void) override;

		void	align(const FrameContext &ctx);

	private:
};

}
