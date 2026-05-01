/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Render.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/28 18:16:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/Render.hpp"
#include "api/vulkan/PipelineMap.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Buffer.hpp"
#include "ecs/Registry.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Component.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/assets/Texture.hpp"
#include "ecs/assets/Shader.hpp"
#include "core/Engine.hpp"
#include "api/vulkan/Renderer.hpp"
#include "api/vulkan/Sampler.hpp"
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
	setLayouts.push_back(DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								VK_SHADER_STAGE_FRAGMENT_BIT, Sampler::getSampler(device, {}), 1)
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

	auto	sampler = Sampler::getSampler(*_device, {});
	auto	SSBO_d = _registry->buildComponentSet<comp::Transform>(*_device, ctx.descriptorPool);
	if (!SSBO_d)
		return ;

	auto	drawEntities = [&](auto entities, PipelineMap *pipeline) {
		for (auto entity: entities) {
			auto	mesh = _assetManager->get<Geometry>(entities.template get<comp::Model>(entity)->modelName);
			if (!mesh)	{ continue ; }
			auto	transform = entities.template get<comp::Transform>(entity);

			for (auto &submesh: mesh->submeshes) {
				auto	texture = _assetManager->get<Texture>(mesh->materialPaths[submesh.materialID]);
				if (!texture)	{ continue ; }

				auto	texture_d = DescriptorFactory(*_device)
									.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
										VK_SHADER_STAGE_FRAGMENT_BIT, sampler, 1)
									.build(*ctx.descriptorPool);
				DescriptorWriter(*_device, texture_d.get())
					.writeImage(0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								*texture->image.get(), texture->image->getFormat(), sampler)
					.update();

				drawCommand(renderer, pipeline)
					.addPush(VK_SHADER_STAGE_ALL_GRAPHICS, EntityData{entity, transform.getDenseIndex()})
					.addBinding(SSBO_d->sets[0])
					.addBinding(texture_d->sets[0])
					.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
					.addIndexBuffer(mesh->triangleIndexBuffer->getBuffer(), submesh.triFirstIndex)
					.setVertexCount(submesh.triIndexCount)
					.submit();
			}
		}
	};

	drawEntities(_registry->view<
			include<comp::Transform, comp::Model, comp::SelectedTag>,
			exclude<comp::HideEntityTag>
		>(), _selectedObjectPipeline);
	drawEntities(_registry->view<
			include<comp::Transform, comp::Model>,
			exclude<comp::HideEntityTag, comp::SelectedTag>
		>(), _normalPipeline);
}

}
