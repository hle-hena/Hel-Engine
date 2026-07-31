/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: vec_utils.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/30 14:15:30 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/31 15:13:52                                        */
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

enum class	Order {
	Incr,
	Decr
};

template <typename T, typename Predicate>
auto insert_if(std::vector<T> &vec, const T &value, Predicate pred) 
{
	return vec.insert(std::find_if(vec.begin(), vec.end(), pred), value);
}

template <Order O, typename T>
auto insert(std::vector<T> &vec, const T &value) {
	if constexpr (O == Order::Incr) {
		return insert_if(vec, value, [&value](const T &elem) { 
			return elem >= value; 
		});
	} else if constexpr (O == Order::Decr) {
		return insert_if(vec, value, [&value](const T &elem) { 
			return elem <= value; 
		});
	}
}

}
