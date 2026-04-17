/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: mathUtils.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/11 14:41:39 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/17 11:26:32                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <functional>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/constants.hpp>
#include <ui/ImGui/imgui.h>

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

static inline bool	pointInTriangle(ImVec2 point, ImVec2 A, ImVec2 B, ImVec2 C) {
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

static inline float	getAngle(glm::vec2 center, glm::vec2 pos1,
						glm::vec2 pos2) {
	glm::vec2	v1 = pos1 - center;
	glm::vec2	v2 = pos2 - center;
	float		angle1 = glm::atan(v1.y, v1.x);
	float		angle2 = glm::atan(v2.y, v2.x);
	float		delta = angle2 - angle1;

	if (delta >  glm::pi<float>())	{ delta -= glm::two_pi<float>(); }
	if (delta < -glm::pi<float>())	{ delta += glm::two_pi<float>(); }

	return (delta);
}

}
