/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.cpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/26 18:12:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/23 15:36:41                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/ISystem.hpp"
#include "core/Engine.hpp"
#include "api/vulkan/Renderer.hpp"

namespace	hel::sys {

void	ISystem::init(const EngineContext &engineCtx,
					const FrameContext &frameCtx) {
	_device = engineCtx.device;
	_registry = engineCtx.registry;
	_imagePool = engineCtx.imagePool;

	_frameCtx = frameCtx;
}

PipelineMap	*ISystem::createPipeline(const PipelineMap::Config &config) {
	auto	pipeline = std::unique_ptr<PipelineMap>(new PipelineMap(config));
	pipeline->initDefaultSets({_frameCtx.globalLayout});
	auto	pipelinePtr = pipeline.get();
	_pipelines.push_back(std::move(pipeline));
	return (pipelinePtr);
}

bool	ISystem::bindPipelines(const RendererHandle &pass) const {
	for (auto &pipeline: _pipelines) {
		pass.bindPipeline(pipeline.get(), {});
	}
}

RendererHandle::Draw	ISystem::drawCommand(const RendererHandle &pass,
											VkPipelineLayout layout) const {
	return (pass.drawCommand(layout, {}));
}



}
