/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderSystem.cpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:14:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/27 18:49:35                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/system/RenderSystem.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/Device.hpp"

namespace	hel {

RenderSystem::RenderSystem(Device &device, Registry &registry)
	:	_device{device},
		_registry{registry} {
}

void	RenderSystem::update(VkCommandBuffer commandBuffer, Window &window) {
	auto	pipeline = getPipelineForFormat(window.getFormat());
}

RenderSystem::SystemPipeline	*RenderSystem::getPipelineForFormat(VkFormat format) {
	if (_pipelines.find(format) != _pipelines.end())
		return (_pipelines[format].get());
	auto	pipeline = std::make_unique<SystemPipeline>(*this);
	_pipelines[format] = std::move(pipeline);
	return (pipeline.get());
}

RenderSystem::SystemPipeline::SystemPipeline(RenderSystem &system)
	:	_pipeline{system._device},
		_system{system} {
}

bool	RenderSystem::SystemPipeline::init(void) {
}

bool	RenderSystem::SystemPipeline::createRenderPass(void) {
	VkRenderPassCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	if (vkCreateRenderPass(_system._device.getLogical(), &createInfo, nullptr, &_renderPass))
		return (true);
}

}