/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/21 19:35:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/03 15:32:31                                        */
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
#include <unordered_map>

namespace	hel {

struct	RenderRequest {
	Entity::id									handle;
	ImVec2										origin{0.f, 0.f};
	Image										*mainImage;
	std::unordered_map<std::string, Image *>	secondaryImages{};
};

class	RenderQueue {
	public:
		static void							push(const RenderRequest &request);
		static std::vector<RenderRequest>	flush(void);

	private:
		static std::vector<RenderRequest>	_requests;
};

}
