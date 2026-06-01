/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/01 18:11:45                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <memory>
#include <cstdint>
#include <vector>

#include "ecs/systems/ISystem.hpp"

namespace hel {

struct	SystemManager {
	using SysPtr		= std::unique_ptr<sys::ISystem>;
	using UnderlyingVec	= std::vector<SysPtr>;

	void	newRender(void) {
		_renderIndex = 0;
		_postProcessIndex = 0;
	};
	const std::vector<sys::ISystem*>	&getUpdates(void)
		{ return _update; }
	const std::vector<sys::ISystem*>	&getUpdateInteractions(void)
		{ return _uInteraction; }
	const std::vector<sys::ISystem*>	&getRenders(void)
		{ return _render[_renderIndex++]; }
	const std::vector<sys::ISystem*>	&getPostProcess(void)
		{ return _postProcess[_postProcessIndex++]; }
	const std::vector<sys::ISystem*>	&getRenderInteractions(void)
		{ return _rInteraction; }
	const std::vector<SysPtr>			&getSystems(void)
		{ return _data; }


	template <typename SysType>
	static void	addSystem() {
		auto	sys = _data.emplace_back(std::make_unique<SysType>()).get();
	}

	PASSKEY(EngineKey, Engine)
	static void	sort(EngineKey);
	static void	clear(EngineKey) {
		_data.clear();
	}

	private:
		uint32_t										_renderIndex;
		uint32_t										_postProcessIndex;
		static std::vector<sys::ISystem*>				_update;
		static std::vector<sys::ISystem*>				_uInteraction;
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
