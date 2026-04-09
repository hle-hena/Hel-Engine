/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/09 18:42:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Transform.hpp"
#include "api/vulkan/PipelineMap.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/assets/Geometry.hpp"
#include "platform/window/Window.hpp"

namespace	hel::sys {

void	Transform::init(void) {
	_handles.reserve(12);
	_assetManager = &_registry->getAssetManager();

	PipelineMap::Config	conf{};
	conf.device = _device;
	conf.assetManager = _assetManager;
	conf.shaderPaths = {
			"assets/shaders/basic.vert.spv",
			"assets/shaders/basic.frag.spv"
	};
	conf.initPipelineLayout = initLayout;
	conf.configurePipeline = configurePipeline;
	_simplePipeline = createPipeline(conf);
}

void	Transform::initLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
						std::vector<VkPushConstantRange> &pushConstants) {
	VkPushConstantRange	vertexPush{};
	vertexPush.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	vertexPush.size = sizeof(EntityData);
	pushConstants.push_back(vertexPush);
	setLayouts.push_back(DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.getSetLayout());
}

void	Transform::configurePipeline(PipelineConfig &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);

	VkPipelineColorBlendAttachmentState	attachment{};
	attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
								VK_COLOR_COMPONENT_G_BIT |
								VK_COLOR_COMPONENT_B_BIT |
								VK_COLOR_COMPONENT_A_BIT;
	attachment.blendEnable = VK_FALSE;
	Pipeline::setBlendAttachment(config, 1, attachment);
}

void	Transform::update(const FrameContext &) {
	auto	entities = _registry->view<include<comp::Transform>>();

	for (auto entity: entities) {
		auto	constTransform = entities.get<comp::Transform>(entity);
		if (!constTransform->isDirty)
			continue ;
		auto	transform = constTransform.modify();
		transform->rotation = glm::normalize(transform->rotation);
		glm::mat4	T = glm::translate(glm::mat4(1.f), transform->position);
		glm::mat4	R = glm::mat4_cast(transform->rotation);
		glm::mat4	S = glm::scale(glm::mat4(1.f), transform->scale);
		transform->worldMatrix = T * R * S;
		transform->normalMatrix = glm::transpose(glm::inverse(transform->worldMatrix));
	}
}

void	Transform::renderArrow(const Renderer &renderer, Entity::id entityHandle, Entity::id arrowHandle) {
	auto	mesh = _assetManager->get<Geometry>("assets/models/arrow.obj");
	drawCommand(renderer, _simplePipeline)
		.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
		.addIndexBuffer(mesh->triangleIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32)
		.submit(mesh->triangleVertexCount);
}

void	Transform::renderMove(const Renderer &renderer) {
	if (_handles.empty()) {
		auto	createEntity = [&](const std::string &stringName, const std::string &modelPath) -> Entity::id {
			Entity::id	newHandle = _registry->createEntity();
			_registry->addComponent<comp::Name>(newHandle).modify()->name = stringName;
			_registry->addComponent<comp::Model>(newHandle).modify()->filePath = modelPath;
			_registry->addComponent<comp::Transform>(newHandle).modify();
			_registry->addComponent<comp::HideEntityTag>(newHandle);
			return (newHandle);
		};
		_handles.push_back(createEntity("X-Arrow", "assets/models/arrow.obj"));
		_handles.push_back(createEntity("Y-Arrow", "assets/models/arrow.obj"));
		_handles.push_back(createEntity("Z-Arrow", "assets/models/arrow.obj"));
		_handles.push_back(createEntity("XY-Plane", "assets/models/quad.obj"));
		_handles.push_back(createEntity("YZ-Plane", "assets/models/quad.obj"));
		_handles.push_back(createEntity("ZX-Plane", "assets/models/quad.obj"));
	}

	auto	ctx = renderer.frameContext();
	auto	set = _registry->buildComponentSet<comp::Transform>(*_device, ctx.descriptorPool);
	if (!set)
		return ;
	for (auto entity: _handles) {
		auto	mesh = _assetManager->get<Geometry>(_registry->getComponent<comp::Model>(entity)->filePath);
		auto	transform = _registry->getComponent<comp::Transform>(entity);
		if (!mesh || !transform)	{ continue ; }

		drawCommand(renderer, _simplePipeline)
			.addPush(VK_SHADER_STAGE_ALL_GRAPHICS, EntityData{entity, transform.getDenseIndex()})
			.addBinding(set->sets[0])
			.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
			.addIndexBuffer(mesh->triangleIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32)
			.submit(mesh->triangleVertexCount);
	}
}

void	Transform::renderScale(const Renderer &renderer) {

}

void	Transform::renderRotate(const Renderer &renderer) {

}

void	Transform::renderUI(const Renderer &renderer) {
	auto	ctx = renderer.frameContext();
	auto	focusedEntity = ctx.window->getEntityFocus();
	if (focusedEntity == Entity::NOT_REGISTERED) {
		for (auto &handle: _handles)
			_registry->removeEntity(handle);
		_handles.clear();
		return ;
	}
	switch (_action) {
		case Action::Move:
			renderMove(renderer);
			break ;
		case Action::Scale:
			renderScale(renderer);
			break ;
		default:
			renderRotate(renderer);
			break ;
	}
}

}
