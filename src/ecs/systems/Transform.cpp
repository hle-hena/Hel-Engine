/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/10 16:28:59                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/Transform.hpp"
#include "api/vulkan/PipelineMap.hpp"
#include "api/vulkan/Renderer.hpp"
#include "api/vulkan/Sampler.hpp"
#include "core/Frame.hpp"
#include "core/Queues.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/assets/Texture.hpp"
#include "platform/input/InputState.hpp"
#include "platform/window/Window.hpp"
#include "utils/mathUtils.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <ui/ImGui/imgui.h>
# define GLM_FORCE_RADIANS
#include <glm/common.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <string>
#include <vulkan/vulkan_core.h>
#include "core/SystemManager.hpp"

namespace	hel::sys {

SystemRegistrar<Transform>	reg_TransformSystem;

Transform::Action	Transform::GizmoContext::action = Action::Move;

void	Transform::init(void) {
	updateDeps.provides = "model matrix calculation";
	updateDeps.block.push_back("view matrix calculation");

	renderInterDeps.provides = "render transform gizmo";

	updateInterDeps.provides = "act on the transform gizmo action";

	_assetManager = &_registry->getAssetManager();
	_inputState = &_registry->getInputState();

	{
		PipelineMap::Config	conf{};
		conf.device = _device;
		conf.assetManager = _assetManager;
		conf.shaderPaths = {
				"assets/shaders/gizmo.vert.spv",
				"assets/shaders/gizmo.frag.spv"
		};
		conf.initPipelineLayout = initSimpleLayout;
		conf.configurePipeline = configureSimplePipeline;
		_simplePipeline = createPipeline(conf);
	}
	{
		PipelineMap::Config	conf{};
		conf.device = _device;
		conf.assetManager = _assetManager;
		conf.shaderPaths = {
				"assets/shaders/NDC.vert.spv",
				"assets/shaders/NDC.frag.spv"
		};
		conf.initPipelineLayout = initNDCLayout;
		conf.configurePipeline = configureNDCPipeline;
		_NDCPipeline = createPipeline(conf);
	}
}

void	Transform::initSimpleLayout(Device &device,
						std::vector<VkDescriptorSetLayout> &setLayouts,
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

void	Transform::configureSimplePipeline(PipelineConfig &config) {
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

void	Transform::initNDCLayout(Device &device,
						std::vector<VkDescriptorSetLayout> &setLayouts,
						std::vector<VkPushConstantRange> &pushConstants) {
	VkPushConstantRange	vertexPush{};
	vertexPush.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	vertexPush.size = sizeof(EntityData);
	pushConstants.push_back(vertexPush);
	setLayouts.push_back(DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.getSetLayout());
	setLayouts.push_back(DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								VK_SHADER_STAGE_FRAGMENT_BIT, Sampler::getSampler(device, {}), 1)
							.getSetLayout());
}

void	Transform::configureNDCPipeline(PipelineConfig &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);

	config.depthStencilInfo.depthTestEnable = VK_FALSE;
	config.depthStencilInfo.depthWriteEnable = VK_FALSE;

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

			std::string	name = it->first;
			if (name.find("Icon") != std::string::npos) {
				gizmo._fullyInit = false;
				if (name.find("Move") != std::string::npos)
					gizmo.action = Action::Move;
				else if (name.find("Scale") != std::string::npos)
					gizmo.action = Action::Scale;
				else
					gizmo.action = Action::Rotate;
			} else {
				gizmo._startDrag = true;
				gizmo._dragName = it->first;
			}
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
		float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05f;
		for (auto &[name, entity]: gizmo.handles) {
			if (name.find("Icon") != std::string::npos)
				continue ;
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
	for (auto &[name, entity]: gizmo.handles) {
		if (name.find("Icon") != std::string::npos)
			continue ;
		auto	mesh = _assetManager->get<Geometry>(_registry->getComponent<comp::Model>(entity)->modelName);
		auto	transform = _registry->getComponent<comp::Transform>(entity);
		auto	tint = _registry->getComponent<comp::Tint>(entity);
		if (!mesh)	{ continue ; }

		drawCommand(renderer, _simplePipeline)
			.addPush(VK_SHADER_STAGE_ALL_GRAPHICS, EntityData{entity, transform.getDenseIndex(), tint.getDenseIndex()})
			.addBinding(set->sets[0])
			.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
			.addIndexBuffer(mesh->triangleIndexBuffer->getBuffer())
			.setVertexCount(mesh->triangleVertexCount)
			.submit();
	}
}

void	Transform::renderUI(const Renderer &renderer, GizmoContext &gizmo) {
	{
		auto	moveTint = _registry->getComponent<comp::Tint>(gizmo.handles["Move-Icon"]).modify();
		auto	scaleTint = _registry->getComponent<comp::Tint>(gizmo.handles["Scale-Icon"]).modify();
		auto	rotateTint = _registry->getComponent<comp::Tint>(gizmo.handles["Rotate-Icon"]).modify();

		auto		activeColIm = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
		auto		nonActiveColIm = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
		glm::vec3	activeCol(activeColIm.x, activeColIm.y, activeColIm.z);
		glm::vec3	nonActiveCol(nonActiveColIm.x, nonActiveColIm.y, nonActiveColIm.z);
		moveTint->tint = (gizmo.action == Action::Move ? activeCol : nonActiveCol);
		scaleTint->tint = (gizmo.action == Action::Scale ? activeCol : nonActiveCol);
		rotateTint->tint = (gizmo.action == Action::Rotate ? activeCol : nonActiveCol);
	}

	auto	ctx = renderer.frameContext();
	auto	SSBO_d = _registry->buildComponentSet<comp::Transform, comp::Tint>(*_device, ctx.descriptorPool);
	if (!SSBO_d)
		return ;
	auto	sampler = Sampler::getSampler(*_device, {});
	for (auto &[name, entity]: gizmo.handles) {
		if (name.find("Icon") == std::string::npos)
			continue ;
		auto	mesh = _assetManager->get<Geometry>(_registry->getComponent<comp::Model>(entity)->modelName);
		auto	texture = _assetManager->get<Texture>(_registry->getComponent<comp::Texture>(entity)->filePath);
		auto	transform = _registry->getComponent<comp::Transform>(entity);
		auto	tint = _registry->getComponent<comp::Tint>(entity);
		if (!mesh || !texture)	{ continue ; }

		auto	texture_d = DescriptorFactory(*_device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								VK_SHADER_STAGE_FRAGMENT_BIT, sampler, 1)
							.build(*ctx.descriptorPool);
		DescriptorWriter(*_device, texture_d.get())
			.writeImage(0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					texture->image->getView(ViewConfig().defaultTextureView()),
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, sampler)
			.update();

		auto	draw = drawCommand(renderer, _NDCPipeline)
			.addPush(VK_SHADER_STAGE_ALL_GRAPHICS, EntityData{entity, transform.getDenseIndex(), tint.getDenseIndex()})
			.addBinding(SSBO_d->sets[0])
			.addBinding(texture_d->sets[0])
			.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
			.addIndexBuffer(mesh->triangleIndexBuffer->getBuffer())
			.setVertexCount(mesh->triangleVertexCount);
		DrawQueue::requestDraw(0, std::move(draw), renderInterDeps);
	}
}

void	Transform::renderInteraction(const Renderer &renderer) {
	auto	ctx = renderer.frameContext();
	if (ctx.window->getEntityFocus() == Entity::NOT_REGISTERED
		|| !_registry->isValidHandle(ctx.request->handle))
		return	;
	auto	[it, inserted] = _gizmoContexts.try_emplace(*renderer.frameContext().request,
								this,
								renderer.frameContext().window,
								renderer.frameContext().request->handle);
	auto	&gizmo = it->second;

	if (!gizmo || ctx.window->focusChanged())
		gizmo.initAction();
	if (!gizmo)
		return ;

	registerClick(ctx, gizmo);
	registerDrag(ctx, gizmo);
	renderGizmo(renderer, gizmo);
	renderUI(renderer, gizmo);
}

void	Transform::registerClick(const FrameContext &ctx, GizmoContext &gizmo) {
	auto	entityImg = ctx.request->images["entity layer"];
	auto	camera = _registry->getComponent<comp::Camera>(ctx.request->handle);
	auto	transform = _registry->getComponent<comp::Transform>(ctx.request->handle);
	if (!_inputState->isPressed<input::Mouse>(0) || !entityImg || !camera || !transform)
		return ;
	glm::vec2	viewportOrigin(ctx.request->origin.x, ctx.request->origin.y);
	VkExtent2D	imgExtent = ctx.request->images["mainColor"]->getExtent();
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

bool Transform::GizmoContext::teleportMouse(const FrameContext &ctx) {
	bool		changed = false;
	glm::vec2	renderOrigin = {ctx.request->origin.x, ctx.request->origin.y};
	auto		imgExtent = ctx.request->images["mainColor"]->getExtent();
	glm::vec2	renderExtent = {imgExtent.width, imgExtent.height};
	auto		mousePos = _baseSystem->_inputState->getMousePos() - renderOrigin;
	float		padding = 3.f;
	glm::vec2	newPos = mousePos;

	if (mousePos.x <= 0)
		newPos.x = renderExtent.x - padding;
	else if (mousePos.x >= renderExtent.x)
		newPos.x = padding;

	if (mousePos.y <= 0)
		newPos.y = renderExtent.y - padding;
	else if (mousePos.y >= renderExtent.y)
		newPos.y = padding;

	if (newPos != mousePos) {
		changed = true;
		newPos += renderOrigin;
		glfwSetCursorPos(ctx.window->getWindow(), newPos.x, newPos.y);
		_baseSystem->_inputState->resetMousePos(newPos);
	}
	return (changed);
}

void	Transform::GizmoContext::dragMove(const FrameContext &ctx) {
	if (_startDrag)
		_startDrag = false;
	if (teleportMouse(ctx))
		return ;

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
	if (_startDrag)
		_startDrag = false;
	if (teleportMouse(ctx))
		return ;

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
	static glm::quat	initialRot;
	static glm::vec2	initialMousePos;
	static glm::vec2	rotationCenter;

	auto		focusedTransform = _registry->getComponent<comp::Transform>(_window->getEntityFocus()).modify();
	auto		renderCamera = _registry->getComponent<comp::Camera>(_requestHandle);
	if (_startDrag) {
		initialRot = focusedTransform->rotation;
		initialMousePos = _baseSystem->_inputState->getMousePos();
		auto		renderExtent = ctx.request->images["mainColor"]->getExtent();
		glm::vec2	renderSize  = {renderExtent.width, renderExtent.height};
		glm::vec2	renderOrigin = {ctx.request->origin.x, ctx.request->origin.y};

		glm::vec4	clip = ctx.globalData.viewProjection * glm::vec4(focusedTransform->position, 1.0f);
		glm::vec2	screenSpaceRotationCenter  = (glm::vec2(clip) / clip.w) * 0.5f + 0.5f;

		rotationCenter = renderOrigin + screenSpaceRotationCenter * renderSize;
		_startDrag = false;
	}
	int	axisIndex = 0;
	const char	*axisNames = "XYZ";
	for (auto i = 0; i < 3; i++)
		if (_dragName->find(axisNames[i]) != std::string::npos)
			axisIndex = i;

	glm::vec2	mousePos = _baseSystem->_inputState->getMousePos();
	float		rotationAmount = mathUtils::getAngle(rotationCenter, initialMousePos, mousePos);
	glm::vec3	axis = glm::normalize(glm::vec3(focusedTransform->worldMatrix[axisIndex]));
	glm::vec4 viewSpaceAxis = renderCamera->view * glm::vec4(axis, 0.0f);
	if (viewSpaceAxis.z > 0.0f)
		rotationAmount = -rotationAmount;
	glm::quat	addedRotation = glm::angleAxis(rotationAmount, axis);
	focusedTransform->rotation = glm::normalize(addedRotation * initialRot);
	_baseSystem->updateEntity(ctx.window->getEntityFocus());

	initialMousePos = mousePos;
	initialRot = focusedTransform->rotation;
}

void	Transform::GizmoContext::initMove(void) {
	auto	focusedTransform = _registry->getComponent<comp::Transform>(_window->getEntityFocus());
	auto	requestTransform = _registry->getComponent<comp::Transform>(_requestHandle);
	if (!focusedTransform || !requestTransform)
		return ;
	freeHandles();
	float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05f;

	EntityFactory(this, requestTransform, dist, "X-Arrow")
		.setModel("move_arrow").setTint(1.f, 0.f, 0.f)
		.setOffRot(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 0, 1)))
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Y-Arrow")
		.setModel("move_arrow").setTint(0.f, 1.f, 0.f)
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Z-Arrow")
		.setModel("move_arrow").setTint(0.f, 0.f, 1.f)
		.setOffRot(glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0)))
		.setOffScale(glm::vec3(0.25f));

	EntityFactory(this, requestTransform, dist, "XY-Plane")
		.setModel("quad").setTint(0.8f, 0.8f, 0.f)
		.setOffRot(glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0)))
		.setOffPos({2.f, 0.f, 2.f}).setOffScale(glm::vec3(0.5f));
	EntityFactory(this, requestTransform, dist, "YZ-Plane")
		.setModel("quad").setTint(0.f, 0.8f, 0.8f)
		.setOffRot(glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1)))
		.setOffPos({2.f, 0.f, 2.f}).setOffScale(glm::vec3(0.5f));
	EntityFactory(this, requestTransform, dist, "ZX-Plane")
		.setModel("quad").setTint(0.8f, 0.f, 0.8f)
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
	float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05f;

	EntityFactory(this, requestTransform, dist, "X-Arrow")
		.setModel("scale_arrow").setTint(1.f, 0.f, 0.f)
		.setOffRot(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 0, 1)))
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Y-Arrow")
		.setModel("scale_arrow").setTint(0.f, 1.f, 0.f)
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Z-Arrow")
		.setModel("scale_arrow").setTint(0.f, 0.f, 1.f)
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
	float	dist = glm::distance(focusedTransform->position, requestTransform->position) * 0.05f;

	EntityFactory(this, requestTransform, dist, "X-Torus")
		.setModel("rotate_torus").setTint(1.f, 0.f, 0.f)
		.setOffRot(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 0, 1)))
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Y-Torus")
		.setModel("rotate_torus").setTint(0.f, 1.f, 0.f)
		.setOffScale(glm::vec3(0.25f));
	EntityFactory(this, requestTransform, dist, "Z-Torus")
		.setModel("rotate_torus").setTint(0.f, 0.f, 1.f)
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
	if (_fullyInit) {
		auto	col = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
		EntityFactory(this, "Move-Icon")
			.setModel("icon_space")
			.setTexture("assets/images/moveGizmoIcon.png")
			.setTint(col.x, col.y, col.z)
			.setPos(0.6f, -0.9f).setScale(0.1f, 0.1f);
		EntityFactory(this, "Scale-Icon")
			.setModel("icon_space")
			.setTexture("assets/images/scaleGizmoIcon.png")
			.setTint(col.x, col.y, col.z)
			.setPos(0.75f, -0.9f).setScale(0.1f, 0.1f);
		EntityFactory(this, "Rotate-Icon")
			.setModel("icon_space")
			.setTexture("assets/images/rotateGizmoIcon.png")
			.setTint(col.x, col.y, col.z)
			.setPos(0.9f, -0.9f).setScale(0.1f, 0.1f);
	}
}



Transform::GizmoContext::EntityFactory::EntityFactory(
	Transform::GizmoContext *baseGizmo, transformComp parentTransform,
	float scale, const std::string &entityName)
	:	_baseGizmo(baseGizmo),
		_parentTransform(parentTransform),
		_scale(scale) {
	_handle = _baseGizmo->_registry->createEntity();
	_addedComp = _baseGizmo->_registry->addComponents<comp::Model,
						comp::Texture, comp::Transform, comp::OffsetTransform,
						comp::Tint, comp::HideEntityTag, comp::NonSelectableTag,
						comp::HideEntityInHierarchyTag>(_handle);
	auto	transform = std::get<2>(_addedComp).modify();
	transform->scale = glm::vec3(scale);
	transform->rotation = parentTransform->rotation;
	transform->position = parentTransform->position;
	_baseGizmo->handles[entityName] = _handle;
}

Transform::GizmoContext::EntityFactory::EntityFactory(Transform::GizmoContext *baseGizmo,
	const std::string &entityName)
	:	_baseGizmo(baseGizmo){
	_handle = _baseGizmo->_registry->createEntity();
	_addedComp = _baseGizmo->_registry->addComponents<comp::Model,
						comp::Texture, comp::Transform, comp::OffsetTransform,
						comp::Tint, comp::HideEntityTag, comp::NonSelectableTag,
						comp::HideEntityInHierarchyTag>(_handle);
	_baseGizmo->handles[entityName] = _handle;
}

Transform::GizmoContext::EntityFactory::~EntityFactory(void) {
	_baseGizmo->_baseSystem->updateEntity(_handle);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setTint(float r, float g, float b) {
	std::get<4>(_addedComp).modify()->tint = {r, g, b};
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setModel(const std::string &filepath) {
	std::get<0>(_addedComp).modify()->modelName = filepath;
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setOffScale(const glm::vec3 &offScale) {
	auto	offset = std::get<3>(_addedComp).modify();
	offset->scale = offScale;

	auto	transform = std::get<2>(_addedComp).modify();
	transform->scale = glm::vec3(_scale) * offset->scale;
	transform->position = _parentTransform->position + (transform->rotation
							* (offset->pos * transform->scale));
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setOffPos(const glm::vec3 &offPos) {
	auto	offset = std::get<3>(_addedComp).modify();
	offset->pos = offPos;

	auto	transform = std::get<2>(_addedComp).modify();
	transform->position = _parentTransform->position + (transform->rotation
							* (offset->pos * transform->scale));
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setOffRot(const glm::quat &offRot) {
	auto	offset = std::get<3>(_addedComp).modify();
	offset->rotation = offRot;

	auto	transform = std::get<2>(_addedComp).modify();
	transform->rotation = _parentTransform->rotation * offset->rotation;
	transform->position = _parentTransform->position + (transform->rotation
							* (offset->pos * transform->scale));
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setPos(float x, float y) {
	auto	transform = std::get<2>(_addedComp).modify();
	transform->position.x = x;
	transform->position.y = y;
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setScale(float x, float y) {
	auto	transform = std::get<2>(_addedComp).modify();
	transform->scale.x = x;
	transform->scale.y = y;
	return (*this);
}

Transform::GizmoContext::EntityFactory	&
Transform::GizmoContext::EntityFactory::setTexture(const std::string &filePath) {
	std::get<1>(_addedComp).modify()->filePath = filePath;
	return (*this);
}

}
