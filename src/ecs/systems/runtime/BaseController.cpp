/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: BaseController.cpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 18:14:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/30 11:43:05                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/runtime/BaseController.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "platform/input/InputState.hpp"
#include "platform/window/Window.hpp"
#include "core/Frame.hpp"

namespace	hel::sys {

void	BaseController::init(void) {
	_input = &_registry->getInputState();
}

void	BaseController::handleKeyboardInput(Entity::id handle, float deltaTime) {
	auto	constTransform = _registry->getComponent<comp::Transform>(handle);
	auto	constController = _registry->getComponent<comp::Controller>(handle);
	auto	tag = _registry->getComponent<comp::BaseControllerTag>(handle);
	if (!constTransform || !constController || !tag)	{ return ; }

	glm::vec3	upVector = glm::vec3(0., 1., 0.);
	if (auto surface = _registry->getComponent<comp::SurfaceAllignement>(handle))
		upVector = surface->localUp;

	glm::vec3	forwardVec = constTransform->rotation * glm::vec3(0.f, 0.f, -1.f);
	glm::vec3	rightVec   = constTransform->rotation * glm::vec3(1.f, 0.f, 0.f);
	forwardVec = glm::normalize(forwardVec - glm::dot(forwardVec, upVector) * upVector);
	rightVec   = glm::normalize(rightVec - glm::dot(rightVec, upVector) * upVector);

	std::vector<std::pair<int, glm::vec3>>	moveConfig {
		{constController->forwardKey, forwardVec},
		{constController->backwardKey, -forwardVec},
		{constController->rightStrideKey, rightVec},
		{constController->leftStrideKey, -rightVec},
		{constController->upKey, upVector},
		{constController->downKey, -upVector}
	};
	glm::vec3 delta{0.0f};
	bool	moved = false;

	for (const auto& [key, dir] : moveConfig) {
		if (_input->isDown<input::Key>(key)) {
			delta += dir;
			moved = true;
		}
	}

	if (!moved || glm::length(delta) < 0.01)
		return ;
	delta = glm::normalize(delta);
	delta *= constController->movementSpeed * deltaTime;
	if (auto transform = constTransform.modify())
		transform->position += delta;
}

void	BaseController::handleMouseMove(Entity::id handle) {
	if (!_input->mouseMoved())	{ return ; }
	auto	constTransform = _registry->getComponent<comp::Transform>(handle);
	auto	constController = _registry->getComponent<comp::Controller>(handle);
	auto	tag = _registry->getComponent<comp::BaseControllerTag>(handle);
	if (!constTransform || !constController || !tag)	{ return ; }

	int	dx, dy;
	_input->getMouseDelta(dx, dy);
	float	sensitivity = constController->mouseSensivity;

	auto		transform = constTransform.modify();
	glm::vec3	upVector = glm::vec3(0., 1., 0.);
	if (auto allign = _registry->getComponent<comp::SurfaceAllignement>(handle))
		upVector = allign->localUp;
	glm::quat	qYaw = glm::angleAxis(-static_cast<float>(dx) * sensitivity,
					upVector);
	glm::quat	qPitch = glm::angleAxis(-static_cast<float>(dy) * sensitivity,
					transform->rotation * glm::vec3(1, 0, 0));
	transform->rotation = glm::normalize(qYaw * qPitch * transform->rotation);
}

void	BaseController::update(const FrameContext &ctx) {
	auto	window = _input->getFocused();
	if (!window)	{ return ; }

	Entity::id	handle = window->getEntityReference();
	handleMouseMove(handle);
	handleKeyboardInput(handle, ctx.deltaTime);
}

}
