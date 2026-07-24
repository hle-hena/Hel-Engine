/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: CycleEntry.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 16:09:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 14:31:59                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <variant>

#include "core/scheduler/PhaseDependency.hpp"

namespace	hel {

struct	FrameContext;
class	Renderer;

}

namespace	hel::sys {

class	ISystem;

struct	CycleEntry {
	private:
		using UpdateFn = void (ISystem::*)(const FrameContext &);
		using RenderFn = void (ISystem::*)(const Renderer &);

		using AnyFn = std::variant<RenderFn, UpdateFn>;

		sys::ISystem		*_system;
		PhaseDependencies	_dep{};
		AnyFn				_func;

	public:
		CycleEntry(sys::ISystem *system, AnyFn func)
			:	_system(system), _func(func)	{}

		PhaseDependencies	*getDep(void)	{ return &_dep; }

		template <typename T>
		void	execute(const T &arg) {
			std::visit([&](auto fn){
				if constexpr (std::is_invocable_v<decltype(fn), sys::ISystem *, const T&>) {
					(_system->*fn)(arg);
				}
			}, _func);
		}
};

}
