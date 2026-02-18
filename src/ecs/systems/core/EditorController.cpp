/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EditorController.cpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:56:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 18:10:09                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/EditorController.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "platform/input/InputState.hpp"
#include "platform/window/Window.hpp"

namespace	hel::sys {

EditorController::EditorController(Device &device, Registry &registry,
						VkDescriptorSetLayout &setLayout)
	:	ISystem(device, registry, setLayout),
		_input{registry.getInputState()} {
}

EditorController::~EditorController(void) {
}

void	EditorController::handleKeyboardInput(Entity::id handle, float deltaTime) {
	auto	*constTransform = _registry.getComponent<comp::Transform>(handle);
	auto	*constController = _registry.getComponent<comp::Controller>(handle);
	if (!constTransform || !constController)	{ return ; }

	// 1. Calculate the sphere's normal at current position
	// glm::vec3 upVector = glm::vec3(0., 1., 0.);
	glm::vec3 upVector = glm::normalize(constTransform->position);

	// 3. Manual Shortest Arc Quaternion (The logic for glm::rotation)
	float dot = glm::dot(constTransform->localUp, upVector);
	glm::quat alignmentQuat;

	if (dot < -0.999999f) {
		// Edge case: up is exactly opposite (unlikely on a sphere surface)
		alignmentQuat = glm::angleAxis(glm::pi<float>(), glm::vec3(1, 0, 0));
	} else {
		glm::vec3 cross = glm::cross(constTransform->localUp, upVector);
		float s = glm::sqrt((1.0f + dot) * 2.0f);
		alignmentQuat = glm::quat(s * 0.5f, cross.x / s, cross.y / s, cross.z / s);
	}

	// 4. Update the actual transform
	if (auto transform = _registry.modify(constTransform)) {
		// Multiply the alignment by the current rotation to 'tilt' the whole basis
		// This keeps the player's relative 'Look' (yaw/pitch) the same
		transform->rotation = glm::normalize(alignmentQuat * transform->rotation);
		transform->localUp = alignmentQuat * transform->localUp;
	}

	// 5. Derive movement vectors from the NOW-ALIGNED rotation
	glm::vec3 forwardVec = constTransform->rotation * glm::vec3(0.f, 0.f, -1.f);
	glm::vec3 rightVec   = constTransform->rotation * glm::vec3(1.f, 0.f, 0.f);

	// Project movement onto the tangent plane (crucial for movement feel)
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
	bool moved = false;

	for (const auto& [key, dir] : moveConfig) {
		if (_input.isKeyHeld(key)) {
			delta += dir;
			moved = true;
		}
	}

	if (!moved || glm::length(delta) < 0.01)
		return ;
	delta = glm::normalize(delta);
	delta *= constController->movementSpeed * deltaTime;
	if (auto transform = _registry.modify(constTransform))
		transform->position += delta;
}

void EditorController::handleMouseMove(Entity::id handle) {
    if (!_input.mouseMoved()) { return ; }
    auto *constTransform = _registry.getComponent<comp::Transform>(handle);
    auto *constController = _registry.getComponent<comp::Controller>(handle);
    if (!constTransform || !constController) { return ; }

    int dx, dy;
    _input.getMouseDelta(dx, dy);

    if (auto transform = _registry.modify<comp::Transform>(constTransform)) {
        // 1. Get the current "Up" for the player based on their position on the sphere
        // This is the axis we should rotate around for 'Yaw' (looking left/right)
        glm::vec3 worldUp = glm::normalize(transform->position);

        // 2. Calculate Yaw (Left/Right) around the dynamic worldUp
        glm::quat qYaw = glm::angleAxis(-static_cast<float>(dx) * constController->mouseSensivity, worldUp);

        // 3. Calculate Pitch (Up/Down) around the local Right axis
        // You are already doing this correctly!
        glm::vec3 localRight = transform->rotation * glm::vec3(1, 0, 0);
        glm::quat qPitch = glm::angleAxis(-static_cast<float>(dy) * constController->mouseSensivity, localRight);

        // 4. Apply rotations
        // Order matters: Yaw should be applied globally, Pitch locally
        transform->rotation = glm::normalize(qYaw * qPitch * transform->rotation);
    }
}

void	EditorController::update(float deltaTime) {
	auto	window = _input.getFocused();
	if (!window)	{ return ; }

	Entity::id	handle = window->getEntityReference();
	handleMouseMove(handle);
	handleKeyboardInput(handle, deltaTime);
}

}
