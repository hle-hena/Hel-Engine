/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/25 10:21:13                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Camera.hpp"
#include "api/vulkan/Device.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/assets/Shader.hpp"
#include "ecs/assets/Texture.hpp"
#include "platform/window/Window.hpp"
#include "core/Engine.hpp"
#include "api/vulkan/Renderer.hpp"
#include "api/vulkan/Sampler.hpp"

namespace	hel::sys {

void	Camera::init(void) {
	_assetManager = &_registry->getAssetManager();
	{
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = _assetManager;
		config.shaderPaths = {
			"assets/shaders/cameraFrustum.vert.spv",
			"assets/shaders/cameraFrustum.frag.spv"
		};
		config.initPipelineLayout = initFrustumLayout;
		config.configurePipeline = configureFrustumPipeline;
		_frustumPipeline = createPipeline(config);
	}
	{
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = _assetManager;
		config.shaderPaths = {
			"assets/shaders/billboard.vert.spv",
			"assets/shaders/billboard.frag.spv"
		};
		config.initPipelineLayout = initSpriteLayout;
		config.configurePipeline = configureSpritePipeline;
		_spritePipeline = createPipeline(config);
	}
}

void	Camera::initFrustumLayout(Device &, std::vector<VkDescriptorSetLayout> &setLayouts,
								std::vector<VkPushConstantRange> &pushConstant) {
	VkPushConstantRange	vertexPush{};
	vertexPush.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vertexPush.size = sizeof(FrustumPush);
	pushConstant.push_back(vertexPush);
}

void	Camera::configureFrustumPipeline(PipelineConfigInfo &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);
	config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
}

void	Camera::initSpriteLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
								std::vector<VkPushConstantRange> &pushConstant) {
	VkPushConstantRange	vertexPush{};
	vertexPush.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vertexPush.size = sizeof(SpritePush);
	pushConstant.push_back(vertexPush);
	auto	set = DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								VK_SHADER_STAGE_FRAGMENT_BIT, Sampler::getSampler(device, {}), 1)
							.getSetLayout();
	setLayouts.push_back(set);
}

void	Camera::configureSpritePipeline(PipelineConfigInfo &config) {
	config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
}

void	Camera::update(const FrameContext &) {
	auto	entities = _registry->view<comp::Transform, comp::Camera>();

	for (auto entity: entities) {
		auto	*constTransform = entities.get<comp::Transform>(entity);
		auto	*constCamera = entities.get<comp::Camera>(entity);

		if (!constCamera->isDirty && !constTransform->isDirty)
			continue ;
		if (auto camera = _registry->modify(constCamera)) {
			glm::mat4 rotate = glm::mat4_cast(glm::conjugate(constTransform->rotation));
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), -constTransform->position);
			glm::mat4 view = rotate * translate;

			camera->view = view;
		}
	}
}

void	Camera::render(const FrameContext &ctx, const Renderer &renderer) {
	auto	selfHandle = ctx.request->handle;
	auto	*selfCam = _registry->getComponent<comp::Camera>(selfHandle);
	auto	*selfTransform = _registry->getComponent<comp::Transform>(selfHandle);
	if (!selfCam || !selfTransform)
		return ;
	auto	commandBuffer = ctx.commandBuffer;
	if (!commandBuffer)	{ return ; }

	auto	entities = _registry->view<comp::Camera,
									comp::Transform>();
	for (auto entity : entities) {
		if (entity == selfHandle)	{ continue ; }
		auto	mesh = _assetManager->get<FullGeometry>("assets/models/frustum.obj");
		if (!mesh)	{ continue ; }
		auto	*transform = entities.get<comp::Transform>(entity);
		auto	*camera = entities.get<comp::Camera>(entity);

		glm::mat4 projection = glm::perspective(glm::radians(camera->fov), 1.f, camera->near, camera->far);
		projection[1][1] *= -1;
		drawCommand(renderer, _frustumPipeline)
			.addPush(VK_SHADER_STAGE_VERTEX_BIT,
					FrustumPush{transform->worldMatrix,
					glm::inverse(projection * camera->view)})
			.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
			.addIndexBuffer(mesh->lineIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32)
			.submit(mesh->lineVertexCount);

		auto	sampler = Sampler::getSampler(*_device, {});
		auto	set = DescriptorFactory(*_device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								VK_SHADER_STAGE_FRAGMENT_BIT, sampler, 1)
							.build(*ctx.descriptorPool);
		auto	texture = _assetManager->get<Texture>("assets/images/cameraSprite.png");
		DescriptorWriter(*_device, set.get())
			.writeImage(0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						*texture->image.get(), texture->image->getFormat(), sampler)
			.update();
		float	size = 0.05f * glm::distance(transform->position, selfTransform->position);
		drawCommand(renderer, _spritePipeline)
			.addBinding(set->sets[0])
			.addPush(VK_SHADER_STAGE_VERTEX_BIT, SpritePush{transform->position, size})
			.submitNoVertex(4);
	}
}

}
