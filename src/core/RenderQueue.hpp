/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/21 19:35:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/21 19:46:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "ecs/Entity.hpp"
# include "api/vulkan/Image.hpp"

# include <span>

namespace	hel {

struct	RenderRequest {
	Entity::id	handle;
	Image		*img;
};

class	RenderQueue {
	public:
		static void							push(const RenderRequest &request);
		static std::vector<RenderRequest>	flush(void);

	private:
		static std::vector<RenderRequest>	_requests;
};

}
