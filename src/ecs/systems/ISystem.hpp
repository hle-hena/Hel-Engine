/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 14:44:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/05 14:17:24                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "api/vulkan/PipelineMap.hpp"
# include "api/vulkan/Renderer.hpp"
# include "core/PhaseDependancy.hpp"
# include "core/Frame.hpp"
# include "ecs/Entity.hpp"

# include <vulkan/vulkan.h>
# include <vector>
# include <memory>

namespace	hel {

class	Device;
class	Registry;
class	ImagePool;
struct	EngineContext;
struct	FrameContext;

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

		virtual void	updateInteraction(const FrameContext &) {}
		virtual void	update(const FrameContext &) {}

		virtual void	render(const Renderer &) {}
		virtual void	postProcessing(const Renderer &) {}
		virtual void	renderInteraction(const Renderer &) {}
		
		virtual std::span<const std::string_view>	getRenderTypes(void) const {
			static constexpr std::array<std::string_view, 0>	types{};
			return types;
		}
		#define RENDER_TYPES(...)												\
		std::span<const std::string_view>	getRenderTypes() const override {	\
			static constexpr std::array<										\
				std::string_view,												\
				std::initializer_list<std::string_view>{__VA_ARGS__}.size()>	\
								types {__VA_ARGS__};							\
			return types;														\
		}

		PhaseDependencies	updateDeps;
		PhaseDependencies	updateInterDeps;
		PhaseDependencies	renderDeps;
		PhaseDependencies	postProcessDeps;
		PhaseDependencies	renderInterDeps;

	protected:
		virtual PipelineMap	*createPipeline(const
								PipelineMap::Config &config) final;

		virtual Renderer::Draw	drawCommand(const Renderer &renderer,
											PipelineMap *pipeline) const final;

		Device										*_device;
		Registry									*_registry;
		ImagePool									*_imagePool;
		std::vector<std::unique_ptr<PipelineMap>>	_pipelines;

	private:
		const FrameContext	*_frameCtx;
};

}
