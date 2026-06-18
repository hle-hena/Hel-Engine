/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 14:44:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/18 10:10:26                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "api/vulkan/PipelineMap.hpp"
# include "core/PhaseDependancy.hpp"

# include <variant>
# include <vulkan/vulkan.h>
# include <vector>
# include <memory>
# include <span>

namespace	hel {

class	Device;
class	Registry;
class	ImagePool;
struct	EngineContext;
struct	FrameContext;
struct	DrawCall;
class	Renderer;

}

namespace	hel::sys {

class	ISystem {
	public:
		ISystem(void) = default;
		virtual ~ISystem(void) = default;

		ISystem(const ISystem &other) = delete;
		ISystem	&operator=(const ISystem &other) = delete;

		virtual void	init(const EngineContext &engineCtx,
							const FrameContext &frameCtx) final;
		virtual void	init(void) = 0;

		using UpdateFn = void (ISystem::*)(const FrameContext &);
		using RenderFn = void (ISystem::*)(const Renderer &);
		struct	Func {
			private:
				using AnyFn = std::variant<
					sys::ISystem::RenderFn,
					sys::ISystem::UpdateFn
				>;

				sys::ISystem		*_system;
				PhaseDependencies	_dep{};
				AnyFn				_func;

			public:
				Func(sys::ISystem *system, AnyFn func)
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
		std::unordered_map<std::string_view, Func>	updateCycleDep;
		std::unordered_map<std::string_view, Func>	renderCycleDep;

	protected:
		template <typename T>
		Func	*addUpdateDep(std::string_view depName,
							void (T::*fn)(const FrameContext&)) {
			auto	[it, _] = updateCycleDep.emplace(depName,
								Func(this, static_cast<UpdateFn>(fn)));
			return &it->second;
		}
		template <typename T>
		Func	*addRenderDep(std::string_view depName,
							void (T::*fn)(const Renderer&)) {
			auto	[it, _] = renderCycleDep.emplace(depName,
								Func(this, static_cast<RenderFn>(fn)));
			return &it->second;
		}
		template <typename T>
		Func	createRenderFunc(void (T::*fn)(const Renderer&)) {
			return {this, static_cast<RenderFn>(fn)};
		}

		virtual PipelineMap	*createPipeline(const
								PipelineMap::Config &config) final;

		virtual DrawCall	drawCommand(const Renderer &renderer,
											PipelineMap *pipeline) const final;

		Device										*_device;
		Registry									*_registry;
		ImagePool									*_imagePool;

	private:
		std::vector<std::unique_ptr<PipelineMap>>	_pipelines;

		const FrameContext	*_frameCtx;
};

}
