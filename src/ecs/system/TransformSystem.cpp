/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: TransformSystem.cpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/22 15:06:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/23 18:48:46                                        */
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

namespace	hel {

TransformSystem::TransformSystem(Registry &registry)
	:	_registry{registry} {
}

TransformSystem::~TransformSystem(void) {
}

void	TransformSystem::update(void) {
	auto	entities = _registry.view<Transform>();

	for (auto entity: entities) {
		auto	&transform = entities.get<Transform>(entity);
		if (!transform.isDirty)
			continue ;
		glm::mat4	T = glm::translate(glm::mat4(1.f), transform.position);
		glm::mat4	R = glm::mat4_cast(transform.rotation);
		glm::mat4	S = glm::scale(glm::mat4(1.f), transform.scale);
		_registry.update(transform, [&](Transform &t){t.worldMatrix = T * R * S;});
		std::cout << "Updated a transform" << std::endl;
	}
}

}
