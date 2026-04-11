/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/11 17:40:09                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Transform.hpp"
#include "api/vulkan/PipelineMap.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/assets/Geometry.hpp"
#include "platform/window/Window.hpp"
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <vulkan/vulkan_core.h>

namespace	hel::sys {

void	Transform::init(void) {
	_assetManager = &_registry->getAssetManager();

	PipelineMap::Config	conf{};
	conf.device = _device;
	conf.assetManager = _assetManager;
	conf.shaderPaths = {
			"assets/shaders/gizmo.vert.spv",
			"assets/shaders/gizmo.frag.spv"
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
							.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.getSetLayout());
}

void	Transform::configurePipeline(PipelineConfig &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);

	config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;

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

	std::erase_if(_gizmoContexts, [](auto &item){
		auto	&[key, value] = item;
		return (--value._life == 0);
	});
}

void	Transform::updateEntity(Entity::id handle) {
	auto	constTransform = _registry->getComponent<comp::Transform>(handle);
	if (!constTransform)
		return ;
	auto	transform = constTransform.modify();
	transform->rotation = glm::normalize(transform->rotation);
	glm::mat4	T = glm::translate(glm::mat4(1.f), transform->position);
	glm::mat4	R = glm::mat4_cast(transform->rotation);
	glm::mat4	S = glm::scale(glm::mat4(1.f), transform->scale);
	transform->worldMatrix = T * R * S;
	transform->normalMatrix = glm::transpose(glm::inverse(transform->worldMatrix));
}

void	Transform::renderMove(const Renderer &renderer, GizmoContext &gizmo) {
	if (!gizmo)	{ return ; }
	gizmo._life++;
	auto	focusedTransform = _registry->getComponent<comp::Transform>(gizmo._window->getEntityFocus());
	auto	requestTransform = _registry->getComponent<comp::Transform>(gizmo._requestHandle);
	if (!focusedTransform || !requestTransform)	{ return ; }

	if (focusedTransform->isDirty || requestTransform->isDirty) {
		float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05;
		for (auto entity: gizmo.handles) {
			auto	transform = _registry->getComponent<comp::Transform>(entity).modify();
			auto	offset = _registry->getComponent<comp::OffsetTransform>(entity);
			transform->scale = glm::vec3(dist) * offset->scale;
			transform->rotation = focusedTransform->rotation * offset->rotation;
			transform->position = focusedTransform->position + (transform->rotation * (offset->pos * transform->scale));
			updateEntity(entity);
		}
	}

	auto	ctx = renderer.frameContext();
	auto	set = _registry->buildComponentSet<comp::Transform, comp::Tint>(*_device, ctx.descriptorPool);
	if (!set)
		return ;
	for (auto entity: gizmo.handles) {
		auto	mesh = _assetManager->get<Geometry>(_registry->getComponent<comp::Model>(entity)->filePath);
		auto	transform = _registry->getComponent<comp::Transform>(entity);
		auto	tint = _registry->getComponent<comp::Tint>(entity);
		if (!mesh)	{ continue ; }

		drawCommand(renderer, _simplePipeline)
			.addPush(VK_SHADER_STAGE_ALL_GRAPHICS, EntityData{entity, transform.getDenseIndex(), tint.getDenseIndex()})
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
	if (ctx.window->getEntityFocus() == Entity::NOT_REGISTERED)
		return	;
	auto	[it, inserted] = _gizmoContexts.try_emplace(*renderer.frameContext().request,
								this,
								renderer.frameContext().window,
								renderer.frameContext().request->handle);
	auto	&gizmo = it->second;

	if (!gizmo || ctx.window->focusChanged())
		gizmo.initAction();
	switch (gizmo.action) {
		case Action::Move:
			renderMove(renderer, gizmo);
			break ;
		case Action::Scale:
			renderScale(renderer);
			break ;
		default:
			renderRotate(renderer);
			break ;
	}
}



Transform::GizmoContext::GizmoContext(Transform *baseSystem, Window *window,
									Entity::id requestHandle)
	:	_baseSystem(baseSystem),
		_window(window),
		_requestHandle(requestHandle) {
	_registry = _baseSystem->_registry;
	initAction();
}

Transform::GizmoContext::~GizmoContext(void) {
	freeHandles();
}

void	Transform::GizmoContext::freeHandles(void) {
	for (auto entity: handles)
		_registry->removeEntity(entity);
	handles.clear();
}

void	Transform::GizmoContext::initMove(void) {
	auto	focusedTransform = _registry->getComponent<comp::Transform>(_window->getEntityFocus());
	auto	requestTransform = _registry->getComponent<comp::Transform>(_requestHandle);
	if (!focusedTransform || !requestTransform)
		return ;
	freeHandles();
	float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05;
	auto	createEntity = [&](const std::string &stringName,
							const std::string &modelPath,
							const glm::quat &offRotation = glm::quat(),
							const glm::vec3 &offPosition = glm::vec3(),
							const glm::vec3 &offScale = glm::vec3(1.f),
							const glm::vec3 &tint = glm::vec3{1.f}){
		Entity::id	newHandle = _registry->createEntity();
		_registry->addComponent<comp::Name>(newHandle).modify()->name = stringName;
		_registry->addComponent<comp::Model>(newHandle).modify()->filePath = modelPath;
		auto	transform = _registry->addComponent<comp::Transform>(newHandle).modify();
		transform->scale = glm::vec3(dist) * offScale;
		transform->rotation = focusedTransform->rotation * offRotation;
		transform->position = focusedTransform->position + (transform->rotation * (offPosition * transform->scale));
		auto	offset = _registry->addComponent<comp::OffsetTransform>(newHandle).modify();
		offset->rotation = offRotation;
		offset->pos = offPosition;
		offset->scale = offScale;
		_registry->addComponent<comp::HideEntityTag>(newHandle);
		_registry->addComponent<comp::Tint>(newHandle).modify()->tint = tint;
		_baseSystem->updateEntity(newHandle);
		handles.push_back(newHandle);
	};
	createEntity("X-Arrow", "assets/models/arrow.obj",
			glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 0, 1)),
			{},
			glm::vec3(5.f),
			{1.f, 0.f, 0.f});
	createEntity("Y-Arrow", "assets/models/arrow.obj",
			glm::quat(1, 0, 0, 0),
			{},
			glm::vec3(5.f),
			{0.f, 0.8f, 0.f});
	createEntity("Z-Arrow", "assets/models/arrow.obj",
			glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0)),
			{},
			glm::vec3(5.f),
			{0.f, 0.f, 0.8f});
	createEntity("XY-Plane", "assets/models/quad.obj",
			glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0)),
			{2.f, 0.f, 2.f},
			glm::vec3(1.f),
			{0.8f, 0.8f, 0.f});
	createEntity("YZ-Plane", "assets/models/quad.obj",
			glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1)),
			{2.f, 0.f, 2.f},
			glm::vec3(1.f),
			{0.f, 0.8f, 0.8f});
	createEntity("ZX-Plane", "assets/models/quad.obj",
			glm::quat(1, 0, 0, 0),
			{2.f, 0.f, 2.f},
			glm::vec3(1.f),
			{0.8f, 0.f, 0.8f});
	_fullyInit = true;
}

void	Transform::GizmoContext::initAction(void) {
	_fullyInit = false;
	switch (action) {
		case Action::Move:
			initMove();
			break ;
		case Action::Scale:
			break ;
		default:
			break ;
	}
}

}
