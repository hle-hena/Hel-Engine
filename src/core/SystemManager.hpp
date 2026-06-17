/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/15 17:58:25                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <memory>
#include <vector>

#include "ecs/systems/ISystem.hpp"

namespace hel {

struct	SystemManager {
	using SysPtr = std::unique_ptr<sys::ISystem>;
	using FuncVec = std::vector<sys::ISystem::Func*>;

	const FuncVec				&getUpdates(void)
		{ return _updateCycle; }
	const std::vector<FuncVec>	&getRenders(std::string_view layer)
		{ return _renderCycle[layer]; }
	const std::vector<SysPtr>	&getSystems(void)
		{ return _data; }


	template <typename SysType>
	static void	addSystem() {
		_data.emplace_back(std::make_unique<SysType>()).get();
	}

	PASSKEY(EngineKey, Engine)
	static void	sort(EngineKey);
	static void	clear(EngineKey) {
		_data.clear();
	}

	private:
		static FuncVec									_updateCycle;
		static std::unordered_map<
					std::string_view,
					std::vector<FuncVec>>				_renderCycle;

		static std::vector<SysPtr>						_data;
};

template <typename SysType>
struct	SystemRegistrar {
	SystemRegistrar() {
		SystemManager::addSystem<SysType>();
	}
};

}
