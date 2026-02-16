/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: CameraSystem.cpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 11:50:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/16 15:01:09                                        */
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

CameraSystem::CameraSystem(Device &device, Registry &registry, VkDescriptorSetLayout &setLayout)
	:	ISystem(device, registry, setLayout) {
}

CameraSystem::~CameraSystem(void) {
}

void	CameraSystem::update(void) {
	auto	entities = _registry.view<Transform, Camera>();

	for (auto entity: entities) {
		auto	*constTransform = entities.get<Transform>(entity);
		auto	*constCamera = entities.get<Camera>(entity);

		if (!constCamera->isDirty && !constTransform->isDirty)
			continue ;
		if (auto camera = _registry.modify(constCamera)) {

			glm::mat4 rotate = glm::mat4_cast(glm::conjugate(constTransform->rotation));
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), -constTransform->position);
			glm::mat4 view = rotate * translate;

			glm::mat4 projection = glm::perspective(camera->fov, camera->aspect, camera->near, camera->far);
			projection[1][1] *= -1; 

			camera->viewProjection = projection * view;
		}
	}
}

}
