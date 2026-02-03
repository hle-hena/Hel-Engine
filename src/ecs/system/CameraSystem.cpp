/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: CameraSystem.cpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 11:50:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/03 11:45:41                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/system/CameraSystem.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "platform/window/Window.hpp"

namespace	hel {

CameraSystem::CameraSystem(Registry &registry)
	:	_registry{registry} {
}

CameraSystem::~CameraSystem(void) {
}

void	CameraSystem::handleInput(void) {
	auto	&inputState = _registry.getInputState();
	auto	*windowFocused = inputState.getFocused();
	if (!windowFocused || !inputState.mouseMoved())	{ return ; }

	if (auto transform = _registry.modify<Transform>(windowFocused->getEntityReference())) {
		int	deltaX, deltaY = 0;
		inputState.getMouseDelta(deltaX, deltaY);
		transform->position.x += deltaX * 0.01;
		transform->position.y += deltaY * 0.01;
	}
}

void	CameraSystem::update(void) {
	handleInput();

	auto	entities = _registry.view<Transform, Camera>();

	for (auto entity: entities) {
		auto	*transform = entities.get<Transform>(entity);
		auto	*camera = entities.get<Camera>(entity);

		if (!camera->isDirty && !transform->isDirty)
			continue ;
		glm::mat4	view = glm::lookAt(transform->position,
			glm::vec3(0.f),
									// transform->position + camera->direction,
									camera->up);
		glm::mat4	projection = glm::perspective(camera->fov, camera->aspect,
												camera->near, camera->far);
		_registry.modify(camera)->viewProjection = projection * view;
	}
}

}
