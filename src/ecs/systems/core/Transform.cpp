/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/16 19:34:51                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Transform.hpp"
#include "api/vulkan/PipelineMap.hpp"
#include "core/Frame.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/assets/Geometry.hpp"
#include "platform/input/InputState.hpp"
#include "platform/window/Window.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <string>
#include <vulkan/vulkan_core.h>

namespace	hel::sys {

void	Transform::init(void) {
	_assetManager = &_registry->getAssetManager();
	_inputState = &_registry->getInputState();

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
}

void	Transform::updateInteraction(const FrameContext &ctx) {
	std::erase_if(_gizmoContexts, [&](auto &gizmoIt){
		auto	&[key, gizmo] = gizmoIt;
		if (--gizmo._life == 0)
			return (true);

		if (gizmo._read.has_value() && gizmo._read->frameIndex == ctx.frameIndex) {
			uint32_t	*data = static_cast<uint32_t *>(gizmo._read->buffer->getMapped());

			Entity::id	handle = data[0];
			auto it = std::find_if(gizmo.handles.begin(), gizmo.handles.end(), [&](const auto &pair){
				return (pair.second == handle);
			});
			gizmo._read.reset();
			if (it == gizmo.handles.end())
				return (false);

			glfwSetInputMode(ctx.window->getWindow(),
						GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			gizmo._dragName = it->first;
		}
		return (false);
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

void	Transform::renderGizmo(const Renderer &renderer, GizmoContext &gizmo) {
	if (!gizmo)	{ return ; }
	gizmo._life++;
	auto	focusedTransform = _registry->getComponent<comp::Transform>(gizmo._window->getEntityFocus());
	auto	requestTransform = _registry->getComponent<comp::Transform>(gizmo._requestHandle);
	if (!focusedTransform || !requestTransform)	{ return ; }

	if (focusedTransform->isDirty || requestTransform->isDirty) {
		float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05;
		for (auto &[key, entity]: gizmo.handles) {
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
	for (auto &[key, entity]: gizmo.handles) {
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

void	Transform::renderInteraction(const Renderer &renderer) {
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

	registerClick(ctx, gizmo);
	registerDrag(ctx, gizmo);
	renderGizmo(renderer, gizmo);
}

void	Transform::registerClick(const FrameContext &ctx, GizmoContext &gizmo) {
	auto	entityImg = ctx.request->secondaryImages["entityID"];
	auto	camera = _registry->getComponent<comp::Camera>(ctx.request->handle);
	auto	transform = _registry->getComponent<comp::Transform>(ctx.request->handle);
	if (!_inputState->isPressed<input::Mouse>(0) || !entityImg || !camera || !transform)
		return ;
	glm::vec2	viewportOrigin(ctx.request->origin.x, ctx.request->origin.y);
	VkExtent2D	imgExtent = ctx.request->mainImage->getExtent();
	glm::vec2	viewportSize(imgExtent.width, imgExtent.height);
	auto	pos = glm::vec2(_inputState->getMousePos() - viewportOrigin);
	if (pos.x < 0 || pos.y < 0 || pos.x > viewportSize.x || pos.y > viewportSize.y)
		return ;

	gizmo._read = Read::Queue::newRequest<uint32_t>(ctx.frameIndex)
			.setSrcImage(entityImg)
			.setOffset({(int32_t)pos.x, (int32_t)pos.y, 0})
			.setExtent({1, 1, 1})
			.push(*_device);
}

void	Transform::registerDrag(const FrameContext &ctx, GizmoContext &gizmo) {
	if (!gizmo._dragName.has_value())
		return ;
	if (_inputState->isReleased<input::Mouse>(0)) {
		glfwSetInputMode(ctx.window->getWindow(),
					GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		gizmo._dragName.reset();
		return ;
	}

	switch (gizmo.action) {
		case Action::Move:
			gizmo.dragMove(ctx);
			break;
		case Action::Scale:
			gizmo.dragScale(ctx);
			break;
		default:
			gizmo.dragRotate(ctx);
			break;
	};
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
	for (auto &[key, entity]: handles)
		_registry->removeEntity(entity);
	handles.clear();
}

void	Transform::GizmoContext::dragMove(const FrameContext &) {
	auto	focusedTransform = _registry->getComponent<comp::Transform>
											(_window->getEntityFocus());
	auto	requestTransform = _registry->getComponent<comp::Transform>
											(_requestHandle);
	auto	requestCamera = _registry->getComponent<comp::Camera>
											(_requestHandle);
	float	multiplier = GIZMO_SENSIBILITY * std::max(1.f, glm::distance(
					focusedTransform->position, requestTransform->position));

	glm::vec3	right = {requestCamera->view[0][0], requestCamera->view[1][0],
						requestCamera->view[2][0]};
	glm::vec3	up = {requestCamera->view[0][1], requestCamera->view[1][1],
						requestCamera->view[2][1]};
	glm::vec2	mouseDelta = _baseSystem->_inputState->getMouseDelta();
	const char	*axisNames = "XYZ";

	std::vector<int>	activeAxes;
	for (int i = 0; i < 3; i++)
		if (_dragName->find(axisNames[i]) != std::string::npos)
			activeAxes.push_back(i);

	glm::vec3 finalOffset(0.0f);
	if (activeAxes.size() == 1) {
		glm::vec3	moveDir = glm::normalize(
								focusedTransform->worldMatrix[activeAxes[0]]);
		float		moveAmount = (mouseDelta.x * glm::dot(moveDir, right) - 
							mouseDelta.y * glm::dot(moveDir, up)) * multiplier;
		finalOffset = moveDir * moveAmount;
	} else if (activeAxes.size() == 2) {
		glm::vec3	axisA = glm::normalize(
								focusedTransform->worldMatrix[activeAxes[0]]);
		glm::vec3	axisB = glm::normalize(
								focusedTransform->worldMatrix[activeAxes[1]]);
		auto		projectToScreen = [&](glm::vec3 dir) -> glm::vec2 {
			return (glm::vec2(glm::dot(dir, right), -glm::dot(dir, up)));
		};

		glm::vec2	projA = projectToScreen(axisA);
		glm::vec2	projB = projectToScreen(axisB);

		float	det = projA.x * projB.y - projA.y * projB.x;
		if (std::abs(det) > 0.0001f) {
			float	u = (mouseDelta.x * projB.y - mouseDelta.y * projB.x) / det;
			float	v = (projA.x * mouseDelta.y - projA.y * mouseDelta.x) / det;
			finalOffset = (axisA * u + axisB * v) * multiplier * 2.f;
		}
	}

	focusedTransform.modify()->position += finalOffset;
	_baseSystem->updateEntity(_window->getEntityFocus());
}

void	Transform::GizmoContext::dragScale(const FrameContext &ctx) {
	auto	focusedTransform = _registry->getComponent<comp::Transform>(_window->getEntityFocus());
	auto	requestTransform = _registry->getComponent<comp::Transform>(_requestHandle);
	auto	requestCamera = _registry->getComponent<comp::Camera>(_requestHandle);

	float	multiplier = GIZMO_SENSIBILITY * std::max(1.f, glm::distance(
					focusedTransform->position, requestTransform->position));
	glm::vec3	camRight = {requestCamera->view[0][0],
							requestCamera->view[1][0],
							requestCamera->view[2][0]};
	glm::vec3	camUp = {requestCamera->view[0][1],
						requestCamera->view[1][1],
						requestCamera->view[2][1]};

	auto		mouseDelta = _baseSystem->_inputState->getMouseDelta();
	const char	*axis = "XYZ";
	glm::vec3	axisVec[3] = {
		{1., 0., 0.},
		{0., 1., 0.},
		{0., 0., 1.}
	};
	glm::vec3	finalOffset(0.0f);

	for (auto i = 0; i < 3; i++) {
		if (_dragName->find(axis[i]) == std::string::npos)
			continue ;
		glm::vec3	scaleDir = axisVec[i];
		float		scaleAmount = (mouseDelta.x * glm::dot(scaleDir, camRight) - 
							mouseDelta.y * glm::dot(scaleDir, camUp)) * multiplier;
		finalOffset = scaleDir * scaleAmount;
		break ;
	}

	focusedTransform.modify()->scale += finalOffset;
	_baseSystem->updateEntity(ctx.window->getEntityFocus());
}

void	Transform::GizmoContext::dragRotate(const FrameContext &ctx) {
	std::cout << "In your dream\n";
	(void)ctx;
}

void	Transform::GizmoContext::initMove(void) {
	auto	focusedTransform = _registry->getComponent<comp::Transform>(_window->getEntityFocus());
	auto	requestTransform = _registry->getComponent<comp::Transform>(_requestHandle);
	if (!focusedTransform || !requestTransform)
		return ;
	freeHandles();
	float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05;

	EntityFactory(this, requestTransform, dist, "X-Arrow")
		.setModel("assets/models/move_arrow.obj").setTint(1.f, 0.f, 0.f)
		.setOffRot(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 0, 1)))
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Y-Arrow")
		.setModel("assets/models/move_arrow.obj").setTint(0.f, 1.f, 0.f)
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Z-Arrow")
		.setModel("assets/models/move_arrow.obj").setTint(0.f, 0.f, 1.f)
		.setOffRot(glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0)))
		.setOffScale(glm::vec3(0.25f));

	EntityFactory(this, requestTransform, dist, "XY-Plane")
		.setModel("assets/models/quad.obj").setTint(0.8f, 0.8f, 0.f)
		.setOffRot(glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0)))
		.setOffPos({2.f, 0.f, 2.f}).setOffScale(glm::vec3(0.5f));
	EntityFactory(this, requestTransform, dist, "YZ-Plane")
		.setModel("assets/models/quad.obj").setTint(0.f, 0.8f, 0.8f)
		.setOffRot(glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1)))
		.setOffPos({2.f, 0.f, 2.f}).setOffScale(glm::vec3(0.5f));
	EntityFactory(this, requestTransform, dist, "ZX-Plane")
		.setModel("assets/models/quad.obj").setTint(0.8f, 0.f, 0.8f)
		.setOffPos({2.f, 0.f, 2.f}).setOffScale(glm::vec3(0.5f));
	_fullyInit = true;
	focusedTransform.modify();
}

void	Transform::GizmoContext::initScale(void) {
	auto	focusedTransform = _registry->getComponent<comp::Transform>(_window->getEntityFocus());
	auto	requestTransform = _registry->getComponent<comp::Transform>(_requestHandle);
	if (!focusedTransform || !requestTransform)
		return ;
	freeHandles();
	float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05;

	EntityFactory(this, requestTransform, dist, "X-Arrow")
		.setModel("assets/models/scale_arrow.obj").setTint(1.f, 0.f, 0.f)
		.setOffRot(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 0, 1)))
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Y-Arrow")
		.setModel("assets/models/scale_arrow.obj").setTint(0.f, 1.f, 0.f)
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Z-Arrow")
		.setModel("assets/models/scale_arrow.obj").setTint(0.f, 0.f, 1.f)
		.setOffRot(glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0)))
		.setOffScale(glm::vec3(0.25f));
	_fullyInit = true;
	focusedTransform.modify();
}

void	Transform::GizmoContext::initRotate(void) {
	auto	focusedTransform = _registry->getComponent<comp::Transform>(_window->getEntityFocus());
	auto	requestTransform = _registry->getComponent<comp::Transform>(_requestHandle);
	if (!focusedTransform || !requestTransform)
		return ;
	freeHandles();
	float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05;

	EntityFactory(this, requestTransform, dist, "X-Torus")
		.setModel("assets/models/rotate_torus.obj").setTint(1.f, 0.f, 0.f)
		.setOffRot(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 0, 1)))
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Y-Torus")
		.setModel("assets/models/rotate_torus.obj").setTint(0.f, 1.f, 0.f)
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Z-Torus")
		.setModel("assets/models/rotate_torus.obj").setTint(0.f, 0.f, 1.f)
		.setOffRot(glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0)))
		.setOffScale(glm::vec3(0.25f));
	_fullyInit = true;
	focusedTransform.modify();
}

void	Transform::GizmoContext::initAction(void) {
	_fullyInit = false;
	switch (action) {
		case Action::Move:
			initMove();
			break ;
		case Action::Scale:
			initScale();
			break ;
		default:
			initRotate();
			break ;
	}
}



Transform::GizmoContext::EntityFactory::EntityFactory(
	Transform::GizmoContext *baseGizmo, transformComp &parentTransform,
	float scale, const std::string &entityName)
	:	_baseGizmo(baseGizmo),
		_parentTransform(parentTransform),
		_scale(scale) {
	_handle = _baseGizmo->_registry->createEntity();
	_addedComp = _baseGizmo->_registry->addComponents<comp::Model,
						comp::Transform, comp::OffsetTransform,
						comp::HideEntityTag, comp::HideEntityInHierarchyTag,
						comp::NonSelectableTag, comp::Tint>(_handle);
	auto	transform = std::get<1>(_addedComp).modify();
	transform->scale = glm::vec3(scale);
	transform->rotation = parentTransform->rotation;
	transform->position = parentTransform->position;
	_baseGizmo->handles[entityName] = _handle;
}

Transform::GizmoContext::EntityFactory::~EntityFactory(void) {
	_baseGizmo->_baseSystem->updateEntity(_handle);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setTint(float r, float g, float b) {
	std::get<6>(_addedComp).modify()->tint = {r, g, b};
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setModel(const std::string &filepath) {
	std::get<0>(_addedComp).modify()->filePath = filepath;
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setOffScale(const glm::vec3 &offScale) {
	auto	offset = std::get<2>(_addedComp).modify();
	offset->scale = offScale;

	auto	transform = std::get<1>(_addedComp).modify();
	transform->scale = glm::vec3(_scale) * offset->scale;
	transform->position = _parentTransform->position + (transform->rotation
							* (offset->pos * transform->scale));
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setOffPos(const glm::vec3 &offPos) {
	auto	offset = std::get<2>(_addedComp).modify();
	offset->pos = offPos;

	auto	transform = std::get<1>(_addedComp).modify();
	transform->position = _parentTransform->position + (transform->rotation
							* (offset->pos * transform->scale));
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setOffRot(const glm::quat &offRot) {
	auto	offset = std::get<2>(_addedComp).modify();
	offset->rotation = offRot;

	auto	transform = std::get<1>(_addedComp).modify();
	transform->rotation = _parentTransform->rotation * offset->rotation;
	transform->position = _parentTransform->position + (transform->rotation
							* (offset->pos * transform->scale));
	return (*this);
}

}
