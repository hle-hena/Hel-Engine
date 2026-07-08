/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EditorController.cpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:56:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/07 17:11:19                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "systems/EditorController.hpp"
#include "components/Transform.hpp"
#include "components/Controllers.hpp"

namespace	hel::sys {

SystemRegistrar<EditorController>	reg_EditorControllerSystem;

void	EditorController::init(void) {
	addUpdateDep("input/controller/editor", &EditorController::handleInput);
}

void	EditorController::handleKeyboardInput(Entity::id handle, float deltaTime) {
	auto	constTransform = _registry->getComponent<comp::Transform>(handle);
	auto	constController = _registry->getComponent<comp::Controller>(handle);
	auto	tag = _registry->getComponent<comp::EditorControllerTag>(handle);
	if (!constTransform || !constController || !tag)	{ return ; }

	glm::vec3	upVector = glm::vec3(0., 1., 0.);
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
		if (_inputState->isDown<input::Key>(key)) {
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

void	EditorController::handleMouseMove(Entity::id handle) {
	if (!_inputState->isDown<input::Mouse>(GLFW_MOUSE_BUTTON_RIGHT) ||
		!_inputState->mouseMoved())	{ return ; }
	auto	constTransform = _registry->getComponent<comp::Transform>(handle);
	auto	constController = _registry->getComponent<comp::Controller>(handle);
	auto	tag = _registry->getComponent<comp::EditorControllerTag>(handle);
	if (!constTransform || !constController || !tag)	{ return ; }

	auto	delta = _inputState->getMouseDelta();
	float	sensitivity = constController->mouseSensivity;

	auto		transform = constTransform.modify();
	glm::vec3	upVector = glm::vec3(0., 1., 0.);
	glm::quat	qYaw = glm::angleAxis(-static_cast<float>(delta.x) * sensitivity,
					upVector);
	glm::quat	qPitch = glm::angleAxis(-static_cast<float>(delta.y) * sensitivity,
					transform->rotation * glm::vec3(1, 0, 0));
	transform->rotation = glm::normalize(qYaw * qPitch * transform->rotation);
}

void	EditorController::handleInput(const FrameContext &ctx) {
	auto	window = _inputState->getFocused();
	if (!window)	{ return ; }

	Entity::id	handle = window->getEntityReference();
	handleMouseMove(handle);
	handleKeyboardInput(handle, *ctx.globals->get<float>("delta_t"));
}

}
