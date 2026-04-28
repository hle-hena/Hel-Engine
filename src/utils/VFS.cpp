/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VFS.cpp                                                             */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/27 22:26:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/27 23:36:00                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */


#include "VFS.hpp"
#include <filesystem>
#include <algorithm>

namespace	hel {

namespace fs = std::filesystem;

std::unordered_map<std::string, std::string>	VFS::_pathCache = {};

std::string	VFS::normalizePath(std::string path) {
	std::replace(path.begin(), path.end(), '\\', '/');

	std::transform(path.begin(), path.end(), path.begin(), 
				[](unsigned char c){ return std::tolower(c); });

	return (path);
}

void VFS::load(EngineKey) {
	std::string	root = "assets";
	if (!fs::exists(root))	{ return ; }

	for (const auto& entry : fs::recursive_directory_iterator(root)) {
		if (entry.is_regular_file()) {
			std::string	actualPath = entry.path().string();
			_pathCache[normalizePath(actualPath)] = actualPath;
		}
	}
}

std::string VFS::getFilepath(std::string path) {
	std::string	lowerQuery = normalizePath(path);

	if (_pathCache.find(lowerQuery) != _pathCache.end()) {
		return (_pathCache[lowerQuery]);
	}
	return (path);
}

}