/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 18:03:33                                        */
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

STransform::STransform(Device &device, Registry &registry,
								VkDescriptorSetLayout &setLayout)
	:	ISystem(device, registry, setLayout) {
}

STransform::~STransform(void) {
}

void	STransform::update(float deltaTime) {
	auto	entities = _registry.view<Transform>();

	for (auto entity: entities) {
		auto	*transform = entities.get<Transform>(entity);
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
