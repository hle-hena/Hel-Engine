/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EditorController.cpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:56:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/01 18:02:52                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/EditorController.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "platform/input/InputState.hpp"
#include "platform/window/Window.hpp"
#include "core/Frame.hpp"
#include "core/SystemManager.hpp"

namespace	hel::sys {

SystemRegistrar<EditorController>	reg_EditorControllerSystem;

void	EditorController::init(void) {
	_input = &_registry->getInputState();

	updateDeps.provides = "editor control";
	updateDeps.require.push_back("allign normal to parent");
	updateDeps.block.push_back("model matrix calculation");
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

void	EditorController::handleMouseMove(Entity::id handle) {
	if (!_input->isDown<input::Mouse>(GLFW_MOUSE_BUTTON_RIGHT) ||
		!_input->mouseMoved())	{ return ; }
	auto	constTransform = _registry->getComponent<comp::Transform>(handle);
	auto	constController = _registry->getComponent<comp::Controller>(handle);
	auto	tag = _registry->getComponent<comp::EditorControllerTag>(handle);
	if (!constTransform || !constController || !tag)	{ return ; }

	auto	delta = _input->getMouseDelta();
	float	sensitivity = constController->mouseSensivity;

	auto		transform = constTransform.modify();
	glm::vec3	upVector = glm::vec3(0., 1., 0.);
	glm::quat	qYaw = glm::angleAxis(-static_cast<float>(delta.x) * sensitivity,
					upVector);
	glm::quat	qPitch = glm::angleAxis(-static_cast<float>(delta.y) * sensitivity,
					transform->rotation * glm::vec3(1, 0, 0));
	transform->rotation = glm::normalize(qYaw * qPitch * transform->rotation);
}

void	EditorController::update(const FrameContext &ctx) {
	auto	window = _input->getFocused();
	if (!window)	{ return ; }

	Entity::id	handle = window->getEntityReference();
	handleKeyboardInput(handle, ctx.deltaTime);
	handleMouseMove(handle);
}

}
