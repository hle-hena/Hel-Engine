/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.cpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/26 18:12:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 19:14:29                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/ISystem.hpp"
#include "core/Frame.hpp"
#include "api/vulkan/Renderer.hpp"

namespace	hel::sys {

void	ISystem::init(Device *device, Registry *registry,
					ImagePool *imagePool, InputState *input)
{
	_device = device;
	_registry = registry;
	_imagePool = imagePool;
	_inputState = input;
	init();
}

PipelineMap	*ISystem::createPipeline(const PipelineMap::Config &config) {
	auto	pipeline = std::unique_ptr<PipelineMap>(new PipelineMap(config));
	pipeline->initDefaultSets({Frame::getGlobalLayout()});
	auto	pipelinePtr = pipeline.get();
	_pipelines.push_back(std::move(pipeline));
	return (pipelinePtr);
}

DrawCall	ISystem::drawCommand(const Renderer &renderer,
									PipelineMap *pipeline) const {
	return (renderer.drawCommand(pipeline, {}));
}



}
