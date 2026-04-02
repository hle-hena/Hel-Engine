/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: AssetManager.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 14:31:25 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 20:00:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <memory>
# include <string>
# include <typeindex>
# include <unordered_map>
# include <vector>

namespace	hel {

class	Device;

class	AssetManager {
	public:
		AssetManager(Device &device);

		template <typename Component>
		std::shared_ptr<Component>	get(const std::string &path);

		static std::vector<char>	readFile(const std::string &path);

	private:
		using assetGroup = std::unordered_map<std::string, std::shared_ptr<void>>;
		using assetMap = std::unordered_map<std::type_index, assetGroup>;

		template <typename Component>
		std::shared_ptr<Component>	load(const std::string &path);

		Device		&_device;
		assetMap	_assets;
};

}

#include "ecs/AssetManager.tpp"
