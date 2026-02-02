/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: TransformSystem.cpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 15:06:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 20:28:51                                        */
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

void	TransformSystem::handleInput(void) {
	auto	&inputState = _registry.getInputState();
	auto	*windowFocused = inputState.getFocused();
	if (!windowFocused)
		return ;
	Entity::id	entityHandle = windowFocused->getEntityReference();
	if (inputState.isKeyHeld(GLFW_KEY_A)) {
		_registry.patch<Transform>(entityHandle, [](Transform &t){t.position.x += 0.001;});
		_registry.patch<Camera>(entityHandle, [](Camera &){});
	}
	if (inputState.isKeyHeld(GLFW_KEY_D)) {
		_registry.patch<Transform>(entityHandle, [](Transform &t){t.position.x -= 0.001;});
		_registry.patch<Camera>(entityHandle, [](Camera &){});
	}
	if (inputState.isKeyHeld(GLFW_KEY_W)) {
		_registry.patch<Transform>(entityHandle, [](Transform &t){t.position.y += 0.001;});
		_registry.patch<Camera>(entityHandle, [](Camera &){});
	}
	if (inputState.isKeyHeld(GLFW_KEY_S)) {
		_registry.patch<Transform>(entityHandle, [](Transform &t){t.position.y -= 0.001;});
		_registry.patch<Camera>(entityHandle, [](Camera &){});
	}
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
		std::cout << "Updated a transform" << std::endl;
	}
}

}
