/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: typeHelper.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/28 14:49:29 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/28 15:16:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <algorithm>
#include <cstdint>

namespace	hel {

template <typename T>
concept ToU32_v = std::convertible_to<T, uint32_t>;

template <typename T>
concept ToI32_v = std::convertible_to<T, int32_t>;

template <typename T>
concept ToF32_v = std::convertible_to<T, float>;


inline uint32_t	ToU32(ToU32_v auto val) {
	return static_cast<uint32_t>(val);
}

inline int32_t	ToI32(ToI32_v auto val) {
	return static_cast<int32_t>(val);
}

inline float	ToF32(ToF32_v auto val) {
	return static_cast<float>(val);
}

template <typename ReturnType>
inline ReturnType	max(const auto &valA, const auto &valB)
	requires std::convertible_to<decltype(valA), ReturnType> &&
			std::convertible_to<decltype(valB), ReturnType>
{
	return std::max(static_cast<ReturnType>(valA),
					static_cast<ReturnType>(valB));
}

template <typename ReturnType>
inline ReturnType	min(const auto &valA, const auto &valB)
	requires std::convertible_to<decltype(valA), ReturnType> &&
			std::convertible_to<decltype(valB), ReturnType>
{
	return std::min(static_cast<ReturnType>(valA),
					static_cast<ReturnType>(valB));
}

}
