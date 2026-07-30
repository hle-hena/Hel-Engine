/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: vec_utils.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/30 14:15:30 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/30 14:24:49                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vector>
#include <algorithm>

namespace	hel {

template <typename T>
std::pair<bool, typename std::vector<T>::const_iterator>
	contains(const std::vector<T> &vec, const T &value)
{
	auto	it = std::find(vec.begin(), vec.end(), value);
	return {it != vec.end(), it};
}

template <typename T, typename Predicate>
auto	contains_if(const std::vector<T> &vec, Predicate pred)
{
	auto it = std::find_if(vec.begin(), vec.end(), pred);
	return std::make_pair(it != vec.end(), it);
}

}
