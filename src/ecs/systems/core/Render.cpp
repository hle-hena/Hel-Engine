/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Render.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/30 18:09:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Render.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Buffer.hpp"
#include "ecs/Registry.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Component.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/assets/Shader.hpp"
#include "core/Engine.hpp"
#include "api/vulkan/Renderer.hpp"

namespace	hel::sys {

void	Render::init(void) {
	_assetManager = &_registry->getAssetManager();
	PipelineMap::Config	config;
	config.device = _device;
	config.assetManager = &_registry->getAssetManager();
	config.shaderPaths = {
		"assets/shaders/basic.vert.spv",
		"assets/shaders/basic.frag.spv"
	};
	config.initPipelineLayout = initLayout;
	config.configurePipeline = configurePipeline;
	_pipelines = createPipeline(config);
}

void	Render::initLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
						std::vector<VkPushConstantRange> &pushConstants) {
	VkPushConstantRange	vertexPush{};
	vertexPush.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vertexPush.size = sizeof(PushConstantData);
	pushConstants.push_back(vertexPush);
	setLayouts.push_back(DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.getSetLayout());
}

void	Render::configurePipeline(PipelineConfigInfo &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);
}

void	Render::render(const FrameContext &ctx, const Renderer &renderer) {
	if (!ctx.commandBuffer)
		return ;

	auto	set = _registry->buildComponentSet<comp::Transform>(*_device, ctx.descriptorPool);
	if (!set)
		return ;
	auto	entities = _registry->view<comp::Transform, comp::Model>();
	for (auto entity: entities) {
		auto	mesh = _assetManager->get<Geometry>(entities.get<comp::Model>(entity)->filePath);
		if (!mesh)	{ continue ; }
		auto	transform = entities.get<comp::Transform>(entity);

		drawCommand(renderer, _pipelines)
			.addPush(VK_SHADER_STAGE_VERTEX_BIT, PushConstantData{transform.getDenseIndex()})
			.addBinding(set->sets[0])
			.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
			.addIndexBuffer(mesh->triangleIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32)
			.submit(mesh->triangleVertexCount);
	}
}

}
