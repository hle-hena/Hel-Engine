/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/25 11:34:52                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Selection.hpp"
#include "ecs/Registry.hpp"

namespace	hel::sys {
 
void	Selection::init(void) {
	_assetManager = &_registry->getAssetManager();
	PipelineMap::Config	config;
	config.device = _device;
	config.assetManager = _assetManager;
	config.shaderPaths = {
		"assets/shaders/cameraFrustum.vert.spv",
		"assets/shaders/cameraFrustum.frag.spv"
	};
	config.initPipelineLayout = initLayout;
	config.configurePipeline = configurePipeline;
	_pipeline = createPipeline(config);
}

void	Selection::update(const FrameContext &ctx) {

}

void	Selection::render(const FrameContext &ctx, const Renderer &conf) {

}

void	Selection::initLayout(Device &, std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants) {

}

void	Selection::configurePipeline(PipelineConfigInfo &config) {

}

}
