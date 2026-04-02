/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/21 19:35:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 19:58:14                                        */
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

namespace	hel {

struct	RenderRequest {
	Entity::id	handle;
	ImVec2		origin{0.f, 0.f};
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
