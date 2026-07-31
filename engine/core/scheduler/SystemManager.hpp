/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/31 17:39:50                                        */
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
#include <unordered_map>
#include <string_view>

#include "utils/Setters.hpp"
#include "utils/str_utils.hpp"

namespace hel::sys {

class	ISystem;
struct	CycleEntry;

}

namespace hel {

struct	SystemManager {
	using SysPtr = std::unique_ptr<sys::ISystem>;
	using EntryVec = std::vector<sys::CycleEntry *>;

	const EntryVec				&getUpdates(void)
		{ return _updateCycle; }
	const std::vector<EntryVec>	&getRenders(std::string_view layer)
		{ return _renderCycle[layer]; }
	const std::vector<SysPtr>	&getSystems(void)
		{ return _data; }


	template <typename SysType>
	static void	addSystem(std::string_view) {
		auto	newSys = std::make_unique<SysType>();
		newSys->loadCycleEntry("", getTypeName(typeid(SysType).name()));
		_data.emplace_back(std::make_unique<SysType>());
	}

	PASSKEY(EngineKey, Engine)
	static void	sort(EngineKey);
	static void	clear(EngineKey);

	private:
		static void	splitPasses(const EntryVec &sortedFuncs);

		static EntryVec									_updateCycle;
		static std::unordered_map<
					std::string_view,
					std::vector<EntryVec>>				_renderCycle;

		static std::vector<SysPtr>						_data;
};

template <typename SysType>
struct	SystemRegistrar {
	SystemRegistrar(std::string_view jsonFilepath) {
		SystemManager::addSystem<SysType>(jsonFilepath);
	}
};

}
