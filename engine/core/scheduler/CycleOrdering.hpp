/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: CycleOrdering.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/31 17:50:37 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/31 18:59:52                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vector>
#include <string_view>
#include <utils/json.hpp>

namespace	hel {

struct	CycleOrdering {
	std::vector<std::string_view>	require{};
	std::vector<std::string_view>	block{};
	std::vector<std::string_view>	layers{};
};

}
