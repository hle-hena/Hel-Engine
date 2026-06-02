/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Sprite.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/16 18:25:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/01 17:51:21                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/Sprite.hpp"
#include "api/vulkan/Device.hpp"
#include "core/Frame.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/assets/Shader.hpp"
#include "ecs/assets/Texture.hpp"
#include "core/Engine.hpp"
#include "api/vulkan/Renderer.hpp"
#include "api/vulkan/Sampler.hpp"
#include <vulkan/vulkan_core.h>
#include "core/SystemManager.hpp"

namespace	hel::sys {

SystemRegistrar<Sprite>	reg_SpriteSystem;

void	Sprite::init(void) {
	renderDeps.provides = "render of the sprites";

	_assetManager = &_registry->getAssetManager();
	{
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = _assetManager;
		config.shaderPaths = {
			"assets/shaders/sprite.vert.spv",
			"assets/shaders/sprite.frag.spv"
		};
		config.initPipelineLayout = initLayout;
		config.configurePipeline = configurePipeline;
		_pipeline = createPipeline(config);
	}
}

void	Sprite::initLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
								std::vector<VkPushConstantRange> &pushConstant) {
	VkPushConstantRange	push{};
	push.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	push.size = sizeof(EntityData);
	pushConstant.push_back(push);

	setLayouts.push_back(DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.getSetLayout());
	auto	set = DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								VK_SHADER_STAGE_FRAGMENT_BIT, Sampler::getSampler(device, {}), 1)
							.getSetLayout();
	setLayouts.push_back(set);
}

void	Sprite::configurePipeline(PipelineConfig &config) {
	config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

	VkPipelineColorBlendAttachmentState	attachment{};
	attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
								VK_COLOR_COMPONENT_G_BIT |
								VK_COLOR_COMPONENT_B_BIT |
								VK_COLOR_COMPONENT_A_BIT;
	attachment.blendEnable = VK_FALSE;
	config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	Pipeline::setBlendAttachment(config, 1, attachment);
}

void	Sprite::render(const Renderer &renderer) {
	auto	ctx = renderer.frameContext();

	auto	renderHandle = ctx.request->handle;
	auto	renderCam = _registry->getComponent<comp::Camera>(renderHandle);
	auto	renderTransform = _registry->getComponent<comp::Transform>(renderHandle);
	if (!renderCam || !renderTransform)
		return ;
	auto	commandBuffer = ctx.commandBuffer;
	if (!commandBuffer)	{ return ; }

	auto	entities = _registry->view<
				include<comp::Texture, comp::Transform>,
				exclude<comp::HideEntityTag, comp::Model>>();
	auto	sampler = Sampler::getSampler(*_device, {});
	auto	SSBO_d = _registry->buildComponentSet<comp::Transform, comp::Camera>(*_device, ctx.descriptorPool);
	if (!SSBO_d)
		return ;
	for (auto entity : entities) {
		if (entity == renderHandle)	{ continue ; }
		auto	transform = entities.get<comp::Transform>(entity);
		auto	texture = _assetManager->get<Texture>(entities.get<comp::Texture>
			(entity)->filePath);
		if (!texture)	{ continue ; }

		auto	texture_d = DescriptorFactory(*_device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								VK_SHADER_STAGE_FRAGMENT_BIT, sampler, 1)
							.build(*ctx.descriptorPool);
		DescriptorWriter(*_device, texture_d.get())
			.writeImage(0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						*texture->image.get(), texture->image->getFormat(), sampler)
			.update();

		drawCommand(renderer, _pipeline)
			.addBinding(SSBO_d->sets[0])
			.addBinding(texture_d->sets[0])
			.addPush(VK_SHADER_STAGE_ALL_GRAPHICS, EntityData{entity,
					transform.getDenseIndex(), renderCam.getDenseIndex()})
			.setVertexCount(4)
			.submit();
	}
}

}
