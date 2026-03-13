/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.cpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/26 18:12:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 18:53:57                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/ISystem.hpp"
#include "core/Engine.hpp"
#include "core/Frame.hpp"

namespace	hel::sys {

void	ISystem::init(const EngineContext &engineCtx,
					const FrameContext &frameCtx) {
	_device = engineCtx.device;
	_registry = engineCtx.registry;
	_imagePool = engineCtx.imagePool;

	_engineCtx = engineCtx;
	_frameCtx = frameCtx;
}

PipelineMap	*ISystem::createPipeline(const PipelineMap::Config &config) {
	auto	pipeline = std::unique_ptr<PipelineMap>(new PipelineMap(config));
	pipeline->initDefaultSets({_frameCtx.globalLayout});
	auto	pipelinePtr = pipeline.get();
	_pipelines.push_back(std::move(pipeline));
	return (pipelinePtr);
}



}
