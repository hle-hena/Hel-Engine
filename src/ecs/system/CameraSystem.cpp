/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: CameraSystem.cpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/02 11:50:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 13:49:49                                        */
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

namespace	hel {

CameraSystem::CameraSystem(Registry &registry)
	:	_registry{registry} {
}

CameraSystem::~CameraSystem(void) {
}

void	CameraSystem::update(void) {
	auto	entities = _registry.view<Transform, Camera>();

	for (auto entity: entities) {
		auto	*transform = entities.get<Transform>(entity);
		auto	*camera = entities.get<Camera>(entity);

		std::cout << "Hey ! I exist " << entity << std::endl;
		if (!camera->isDirty)
			continue ;
		glm::mat4	view = glm::lookAt(transform->position,
									transform->position + camera->direction,
									camera->up);
		glm::mat4	projection = glm::perspective(camera->fov, camera->aspect,
												camera->near, camera->far);
		_registry.update(camera, [&](Camera &camera){
			camera.viewProjection = projection * view;
		});
	}
}

}
