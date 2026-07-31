/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.cpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/26 18:12:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/31 17:39:40                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/ecs/ISystem.hpp"
#include "core/Frame.hpp"
#include "rhi/render/Renderer.hpp"

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

void	ISystem::loadCycleEntry(std::string_view jsonFilepath,
					const std::string &systemName) {
	std::cout << "Loading: " << jsonFilepath << "\n" << "System: "
		<< systemName << "\n\n";
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
