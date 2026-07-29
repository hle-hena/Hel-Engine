/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: AssetManager.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 14:31:25 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 17:07:15                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace	hel {

class	Device;

class	AssetManager {
	public:
		void	init(Device *device) { _device = device; };

		template <typename Asset>
		std::shared_ptr<Asset>	get(const std::string &path);

	private:
		using assetGroup = std::unordered_map<std::string, std::shared_ptr<void>>;
		using assetMap = std::unordered_map<std::type_index, assetGroup>;

		template <typename Asset>
		std::shared_ptr<Asset>	load(const std::string &path);

		Device		*_device;
		assetMap	_assets;
};

}

#include "core/ecs/AssetManager.tpp"
