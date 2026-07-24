/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderQueue.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:24:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 14:36:53                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string>
#include <glm/glm.hpp>

#include "core/ecs/Entity.hpp"
#include "utils/Ref.hpp"

namespace	hel {

class	Image;

struct	RenderRequest {
	std::string									requestType;
	Entity::id									handle;
	glm::vec2									origin{0.f, 0.f};
	std::unordered_map<std::string, Ref<Image>>	images{};

	bool	operator==(const RenderRequest &other) const;
	struct	Hasher {
		size_t	operator()(const RenderRequest &request) const;
	};
};


class	RenderQueue {
	public:
		static void		push(const RenderRequest &request) {
			if (!request.images.empty() && request.images.contains("mainColor"))
				_requests.push_back(request);
		}
		static std::vector<RenderRequest>	flush(void) {
			return (std::move(_requests));
		}

	private:
		static std::vector<RenderRequest>	_requests;
};

}
