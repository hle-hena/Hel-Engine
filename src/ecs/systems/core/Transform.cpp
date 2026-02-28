/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 22:51:49                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Transform.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"

namespace	hel::sys {

Transform::Transform(Device &device, Registry &registry)
	:	ISystem(device, registry) {
}

Transform::~Transform(void) {
}

void	Transform::update(float deltaTime) {
	auto	entities = _registry.view<comp::Transform>();

	for (auto entity: entities) {
		auto	*transform = entities.get<comp::Transform>(entity);
		if (!transform || !transform->isDirty)
			continue ;
		glm::mat4	T = glm::translate(glm::mat4(1.f), transform->position);
		glm::mat4	R = glm::mat4_cast(transform->rotation);
		glm::mat4	S = glm::scale(glm::mat4(1.f), transform->scale);
		auto modTransform = _registry.modify(transform);
		modTransform->worldMatrix = T * R * S;
		modTransform->normalMatrix = glm::transpose(glm::inverse(modTransform->worldMatrix));
	}
}

}
