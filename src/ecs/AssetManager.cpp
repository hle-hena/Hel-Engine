/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: AssetManager.cpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 15:19:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/10 16:06:33                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/AssetManager.hpp"

#include <fstream>

namespace	hel {

AssetManager::AssetManager(Device &device)
	:	_device{device} {
}

std::vector<char>	AssetManager::readFile(const std::string& filepath) {
	std::ifstream	file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		return (std::vector<char>(0));

	size_t				fileSize = static_cast<size_t>(file.tellg());
	std::vector<char>	buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return (buffer);
}

}
