/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: str_utils.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/15 17:42:18 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/28 14:09:45                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string_view>
#include <vector>
#include <fstream>
#include <memory>
#include <cxxabi.h>

namespace	hel {

inline std::vector<char>	readFile(const std::string &filepath) {
	std::ifstream	file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		return (std::vector<char>(0));

	size_t				fileSize = static_cast<size_t>(file.tellg());
	std::vector<char>	buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
	file.close();

	return (buffer);
}

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

inline std::string	getTypeName(const char *mangledName) {
	int	status = 0;
	std::unique_ptr<char, void(*)(void*)> res {
		abi::__cxa_demangle(mangledName, nullptr, nullptr, &status),
		std::free
	};

	return (status == 0) ? res.get() : mangledName;
}

}