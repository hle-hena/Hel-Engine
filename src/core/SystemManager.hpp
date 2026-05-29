/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/05/29 18:18:03                                        */
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

	void	newFrame(void) {
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

		_render[0].push_back(sys);
		_postProcess[0].push_back(sys);
		_update.push_back(sys);
		_rInteraction.push_back(sys);
		_uInteraction.push_back(sys);
	}

	PASSKEY(EngineKey, Engine)
	static void	sort(EngineKey);
	static void	clear(EngineKey) {
		_data.clear();
	}

	private:
		template <typename Fn1, typename Fn2>
		static bool overrides(Fn1 derivedFn, Fn2 baseFn) {
			return static_cast<void*>(derivedFn) != static_cast<void*>(baseFn);
		}

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
