/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: CycleEntry.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 16:09:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/08/01 16:05:15                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <variant>

#include "rhi/render/ExecutionContext.hpp"
#include "rhi/render/RenderDependency.hpp"
#include "core/scheduler/CycleOrdering.hpp"

namespace	hel::sys {

class	ISystem;

}

namespace	hel {

class	Renderer;

struct	CycleEntry {
	private:
		using UpdateFn = void (sys::ISystem::*)(const ExecutionContext &);
		using RenderFn = void (sys::ISystem::*)(const Renderer &);

		using AnyFn = std::variant<RenderFn, UpdateFn>;

		sys::ISystem		*_system;
		RenderDependency	_dep{};
		CycleOrdering		_ord{};
		AnyFn				_func;

	public:
		CycleEntry(sys::ISystem *system, AnyFn func)
			:	_system(system), _func(func)	{}

		RenderDependency	*getDep(void)	{ return &_dep; }
		CycleOrdering		*getOrd(void)	{ return &_ord; }

		template <typename T>
		void	execute(const T &arg) {
			std::visit([&](auto fn){
				if constexpr (std::is_invocable_v<decltype(fn), sys::ISystem *, const T&>) {
					(_system->*fn)(arg);
				}
			}, _func);
		}

	friend class sys::ISystem;
};

}
