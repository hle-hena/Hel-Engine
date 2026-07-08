/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: DrawQueue.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:30:42 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 18:37:30                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <cstdint>
#include <vector>
#include <map>

#include "core/PhaseDependency.hpp"
#include "api/vulkan/Renderer.hpp"

namespace	hel {

class	DrawQueue {
	public:
		struct	RequestVector {
			PhaseDependencies		dep;
			std::vector<DrawCall>	draws;
		};
		using InnerMap = std::map<uint32_t, std::vector<RequestVector>>;
		struct	RequestMap {
			public:
				RequestVector	*at(uint32_t levelAsked, const PhaseDependencies &depAsked);
				void			clear(void);
			private:
				InnerMap	_data{};
			
			friend class DrawQueue;
		};

		static void	requestDraw(uint32_t level, DrawCall &&drawCommand,
								PhaseDependencies &dep);
		static InnerMap	flush(void) { return std::move(_requests._data); };

	private:
		static RequestMap	_requests;

	template <typename ReadType>
	friend struct	Builder;
};

}
