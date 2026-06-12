/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/12 15:25:42                                        */
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
	using SysPtr		= std::unique_ptr<sys::ISystem>;
	using UnderlyingVec	= std::vector<SysPtr>;

	const std::vector<sys::ISystem::Func*>	&getUpdates(void)
		{ return _updateCycle; }
	const std::vector<std::vector<sys::ISystem*>>	&getRenders(void)
		{ return _render; }
	const std::vector<std::vector<sys::ISystem*>>	&getPostProcess(void)
		{ return _postProcess; }
	const std::vector<sys::ISystem*>	&getRenderInteractions(void)
		{ return _rInteraction; }
	const std::vector<SysPtr>			&getSystems(void)
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
		static std::vector<sys::ISystem::Func*>			_updateCycle;
		static std::vector<std::vector<
							sys::ISystem::Func*>>		_renderCycle;

		static std::vector<std::vector<sys::ISystem*>>	_render;
		static std::vector<std::vector<sys::ISystem*>>	_postProcess;
		static std::vector<sys::ISystem*>				_rInteraction;
		static UnderlyingVec							_data;
};

template <typename SysType>
struct	SystemRegistrar {
	SystemRegistrar() {
		SystemManager::addSystem<SysType>();
	}
};

}
