/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ControllerSystem.cpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/03 18:56:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/16 15:07:43                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/system/ControllerSystem.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "platform/input/InputState.hpp"
#include "platform/window/Window.hpp"

namespace	hel {

ControllerSystem::ControllerSystem(Device &device, Registry &registry,
						VkDescriptorSetLayout &setLayout)
	:	ISystem(device, registry, setLayout),
		_input{registry.getInputState()} {
}

ControllerSystem::~ControllerSystem(void) {
}

void	ControllerSystem::handleKeyboardInput(Entity::id handle) {
	auto	*constTransform = _registry.getComponent<Transform>(handle);
	auto	*constController = _registry.getComponent<Controller>(handle);
	if (!constTransform || !constController)	{ return ; }

	glm::vec3	forwardVec = constTransform->rotation *
							glm::vec3(0.f, 0.f, -constController->movementSpeed);
	glm::vec3	leftVec = constTransform->rotation *
							glm::vec3(-constController->movementSpeed, 0.f, 0.f);
	std::vector<std::pair<int, glm::vec3>>	moveConfig {
		{constController->forwardKey, forwardVec},
		{constController->backwardKey, -forwardVec},
		{constController->leftStrideKey, leftVec},
		{constController->rightStrideKey, -leftVec}
	};
	glm::vec3 delta{0.0f};
	bool moved = false;

	for (const auto& [key, dir] : moveConfig) {
		if (_input.isKeyHeld(key)) {
			delta += dir;
			moved = true;
		}
	}

	if (!moved)
		return ;
	if (auto transform = _registry.modify(constTransform))
		transform->position += delta;
}

void	ControllerSystem::handleMouseMove(Entity::id handle) {
	if (!_input.mouseMoved())	{ return ; }

	int		dx, dy;
	_input.getMouseDelta(dx, dy);

	if (auto transform = _registry.modify<Transform>(handle)) {
		float sensitivity = 0.001f;

		glm::quat qYaw = glm::angleAxis(-static_cast<float>(dx) * sensitivity, glm::vec3(0, 1, 0));
		glm::vec3 localRight = transform->rotation * glm::vec3(1, 0, 0);
		glm::quat qPitch = glm::angleAxis(-static_cast<float>(dy) * sensitivity, localRight);

		transform->rotation = glm::normalize(qYaw * qPitch * transform->rotation);
	}
}

void	ControllerSystem::update(void) {
	auto	window = _input.getFocused();
	if (!window)	{ return ; }

	Entity::id	handle = window->getEntityReference();
	handleKeyboardInput(handle);
	handleMouseMove(handle);
}

}

