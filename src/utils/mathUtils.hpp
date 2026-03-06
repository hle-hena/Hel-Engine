/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: mathUtils.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/11 14:41:39 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/06 15:25:55                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <functional>

namespace	hel::mathUtils {

template <typename T, typename... Rest>
void	hashCombine(std::size_t &seed, const T &v, const Rest&... rest)
{
	seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

struct	EnumHash {
    template<typename T>
    size_t operator()(T t) const {
        return std::hash<std::underlying_type_t<T>>{}(t);
    }
};

}
