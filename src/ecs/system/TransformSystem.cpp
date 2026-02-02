/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: TransformSystem.cpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 15:06:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 20:50:15                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/system/TransformSystem.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "platform/window/Window.hpp"

namespace	hel {

TransformSystem::TransformSystem(Registry &registry)
	:	_registry{registry} {
}

TransformSystem::~TransformSystem(void) {
}

void TransformSystem::handleInput(void) {
	auto& inputState = _registry.getInputState();
	auto* windowFocused = inputState.getFocused();
	if (!windowFocused) { return ; }

	Entity::id entityHandle = windowFocused->getEntityReference();
	auto* transform = _registry.getComponent<Transform>(entityHandle);
	if (!transform) return ;

	static const std::vector<std::pair<int, glm::vec3>> moveConfig = {
		{GLFW_KEY_A, { 0.001f, 0.0f, 0.0f}},
		{GLFW_KEY_D, {-0.001f, 0.0f, 0.0f}},
		{GLFW_KEY_W, { 0.0f,  0.001f, 0.0f}},
		{GLFW_KEY_S, { 0.0f, -0.001f, 0.0f}}
	};

	glm::vec3 delta{0.0f};
	bool moved = false;

	for (const auto& [key, dir] : moveConfig) {
		if (inputState.isKeyHeld(key)) {
			delta += dir;
			moved = true;
		}
	}

	if (!moved)
		return ;
	if (auto transform = _registry.modify<Transform>(entityHandle))
		transform->position += delta;
}

void	TransformSystem::update(void) {
	handleInput();

	auto	entities = _registry.view<Transform>();

	for (auto entity: entities) {
		auto	*transform = entities.get<Transform>(entity);
		if (!transform || !transform->isDirty)
			continue ;
		glm::mat4	T = glm::translate(glm::mat4(1.f), transform->position);
		glm::mat4	R = glm::mat4_cast(transform->rotation);
		glm::mat4	S = glm::scale(glm::mat4(1.f), transform->scale);
		_registry.modify(transform)->worldMatrix = T * R * S;
	}
}

}
