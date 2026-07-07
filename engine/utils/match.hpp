/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: match.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/07 14:11:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/07 16:10:47                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string_view>

namespace	hel {

inline bool match(std::string_view pattern, std::string_view string) {
	size_t	p = 0, s = 0;
	size_t	starIdx = std::string_view::npos, sBacktrack = 0;

	while (s < string.size()) {
		if (p < pattern.size() && pattern[p] == string[s]) {
			p++; s++;
		} else if (p < pattern.size() && pattern[p] == '*') {
			starIdx = p;
			sBacktrack = s;
			p++;
		} else if (starIdx != std::string_view::npos) {
			p = starIdx + 1;
			s = ++sBacktrack;
		} else {
			return false;
		}
	}
	while (p < pattern.size() && pattern[p] == '*')
		p++;
	return p == pattern.size();
}

inline bool	matchPath(std::string_view pattern, std::string_view string) {
	size_t	p = 0, s = 0;
	size_t	starIdx = std::string_view::npos, sBacktrack = 0;

	while (s < string.size()) {
		if (p == pattern.size() && (string[s] == '/'
				|| (string[s - 1] == '/' && pattern[p - 1] == '/')))
		{
			return true;
		} else if (p < pattern.size() && pattern[p] == string[s]) {
			p++; s++;
		} else if (p < pattern.size() && pattern[p] == '*') {
			starIdx = p;
			sBacktrack = s;
			p++;
		} else if (starIdx != std::string_view::npos && string[sBacktrack] != '/') {
			p = starIdx + 1;
			s = ++sBacktrack;
		} else {
			return false;
		}
	}
	while (p < pattern.size() && pattern[p] == '*')
		p++;
	return p == pattern.size();
}

}