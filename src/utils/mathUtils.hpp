/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: mathUtils.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/11 14:41:39 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/17 20:55:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <functional>
# include "api/ImGui/imgui.h"

namespace	hel::mathUtils {

template <typename T, typename... Rest>
void	hashCombine(std::size_t &seed, const T &v, const Rest&... rest)
{
	seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

struct	EnumHash {
	template<typename T>
	size_t	operator()(T t) const {
		return std::hash<std::underlying_type_t<T>>{}(t);
	}
};

static bool	pointInTriangle(ImVec2 point, ImVec2 A, ImVec2 B, ImVec2 C) {
	auto sign = [](ImVec2 p1, ImVec2 p2, ImVec2 p3) {
		return ((p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y));
	};
	float	d1 = sign(point, A, B);
	float	d2 = sign(point, B, C);
	float	d3 = sign(point, C, A);
	bool	hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	bool	hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);
	return (!(hasNeg && hasPos));
}

}
