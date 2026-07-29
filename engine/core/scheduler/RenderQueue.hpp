/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:24:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/28 18:37:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <glm/glm.hpp>

namespace	hel {

class	Image;
struct	RenderRequest;

class	RenderQueue {
	public:
		static void		push(RenderRequest &&request);
		static std::vector<RenderRequest>	flush(void);

	private:
		static std::vector<RenderRequest>	_requests;
};

}
