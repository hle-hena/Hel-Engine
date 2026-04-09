/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Render.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/09 18:41:41                                        */
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
#include <vulkan/vulkan_core.h>

namespace	hel::sys {

void	Render::init(void) {
	_assetManager = &_registry->getAssetManager();
	{
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = &_registry->getAssetManager();
		config.shaderPaths = {
			"assets/shaders/basic.vert.spv",
			"assets/shaders/basic.frag.spv"
		};
		config.initPipelineLayout = initLayout;
		config.configurePipeline = configureSelectedPipeline;
		_selectedObjectPipeline = createPipeline(config);
	}
	{
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = &_registry->getAssetManager();
		config.shaderPaths = {
			"assets/shaders/basic.vert.spv",
			"assets/shaders/basic.frag.spv"
		};
		config.initPipelineLayout = initLayout;
		config.configurePipeline = configureNormalPipeline;
		_normalPipeline = createPipeline(config);
	}
}

void	Render::initLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
						std::vector<VkPushConstantRange> &pushConstants) {
	VkPushConstantRange	vertexPush{};
	vertexPush.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	vertexPush.size = sizeof(EntityData);
	pushConstants.push_back(vertexPush);
	setLayouts.push_back(DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.getSetLayout());
}

void	Render::configureNormalPipeline(PipelineConfig &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);

	VkPipelineColorBlendAttachmentState	attachment{};
	attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
								VK_COLOR_COMPONENT_G_BIT |
								VK_COLOR_COMPONENT_B_BIT |
								VK_COLOR_COMPONENT_A_BIT;
	attachment.blendEnable = VK_FALSE;
	Pipeline::setBlendAttachment(config, 1, attachment);
}

void	Render::configureSelectedPipeline(PipelineConfig &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);

	config.depthStencilInfo.stencilTestEnable = VK_TRUE;
	config.depthStencilInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;
	config.depthStencilInfo.front.passOp = VK_STENCIL_OP_REPLACE;
	config.depthStencilInfo.front.reference = 1;
	config.depthStencilInfo.front.compareMask = 0xFF;
	config.depthStencilInfo.front.writeMask = 0x1;
	config.depthStencilInfo.back = config.depthStencilInfo.front;

	VkPipelineColorBlendAttachmentState	attachment{};
	attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
								VK_COLOR_COMPONENT_G_BIT |
								VK_COLOR_COMPONENT_B_BIT |
								VK_COLOR_COMPONENT_A_BIT;
	attachment.blendEnable = VK_FALSE;
	Pipeline::setBlendAttachment(config, 1, attachment);
}

void	Render::render(const Renderer &renderer) {
	auto	ctx = renderer.frameContext();
	if (!ctx.commandBuffer)
		return ;

	auto	set = _registry->buildComponentSet<comp::Transform>(*_device, ctx.descriptorPool);
	if (!set)
		return ;
	auto	entities = _registry->view<include<comp::Transform, comp::Model>>();
	for (auto entity: entities) {
		auto	mesh = _assetManager->get<Geometry>(entities.get<comp::Model>(entity)->filePath);
		auto	hidden = _registry->getComponent<comp::HideEntityTag>(entity);
		if (!mesh || hidden)	{ continue ; }
		auto	transform = entities.get<comp::Transform>(entity);

		//TODO -> sort those calls later on.
		auto	drawCall = ctx.window->getEntityFocus() == entity ?
								drawCommand(renderer, _selectedObjectPipeline) :
								drawCommand(renderer, _normalPipeline);
		drawCall.addPush(VK_SHADER_STAGE_ALL_GRAPHICS, EntityData{entity, transform.getDenseIndex()})
			.addBinding(set->sets[0])
			.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
			.addIndexBuffer(mesh->triangleIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32)
			.submit(mesh->triangleVertexCount);
	}
}

}
