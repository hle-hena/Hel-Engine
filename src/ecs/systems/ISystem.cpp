/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.cpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/26 18:12:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/04 11:34:54                                        */
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

size_t	DepHasher::operator()(const PhaseDependencies &dep) const {
	size_t	seed = 0;
	for (auto &write: dep.write)
		mathUtils::hashCombine(seed, write.imageName, write.usage, write.format);
	for (auto &read: dep.read)
		mathUtils::hashCombine(seed, read.imageName, read.usage, read.format);
	return seed;
}

bool	ImageDep::operator==(const ImageDep &o) const {
	return imageName == o.imageName && usage == o.usage && format == o.format;
}

bool	PhaseDependencies::operator==(const PhaseDependencies &o) const {
	return write == o.write && read == o.read;
	//TODO -> actually, check if there is no overlap on the order of the writes,
	// and if there isn't a write image in the read
}

void	ISystem::init(const EngineContext &engineCtx,
					const FrameContext &frameCtx) {
	_device = engineCtx.device;
	_registry = engineCtx.registry;
	_imagePool = engineCtx.imagePool;

	_frameCtx = &frameCtx;
}

PipelineMap	*ISystem::createPipeline(const PipelineMap::Config &config) {
	auto	pipeline = std::unique_ptr<PipelineMap>(new PipelineMap(config));
	pipeline->initDefaultSets({_frameCtx->globalLayout});
	auto	pipelinePtr = pipeline.get();
	_pipelines.push_back(std::move(pipeline));
	return (pipelinePtr);
}

Renderer::Draw	ISystem::drawCommand(const Renderer &renderer,
									PipelineMap *pipeline) const {
	return (renderer.drawCommand(pipeline, {}));
}



}
