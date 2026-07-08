/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SurfaceAllignement.cpp                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 18:20:42 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/07 17:11:07                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "systems/SurfaceAllignement.hpp"
#include "components/Transform.hpp"

namespace	hel::sys {

SystemRegistrar<SurfaceAllignement>	reg_SurfaceAllignementSystem;

void	SurfaceAllignement::init(void) {
	addUpdateDep("general/normal alignment", &SurfaceAllignement::align)
		->getDep()
			->addRequire("input/gizmo")
			->addRequire("input/ui")
			->addBlock("input/controller");
}

void	SurfaceAllignement::align(const FrameContext &) {
	auto	entities = _registry->view<include<comp::Transform, comp::SurfaceAllignement>>();

	for (auto entity: entities) {
		auto	constTransform = entities.get<comp::Transform>(entity);
		auto	constAllign = entities.get<comp::SurfaceAllignement>(entity);

		//TODO -> This just assumes that it doesn't have any parent.
		glm::vec3	up{0.f, 1.f, 0.f};
		if (constAllign->isDynamic)
			up = glm::normalize(constTransform->position);

		glm::quat	allignement;
		float	dot = glm::dot(constAllign->localUp, up);
		if (dot < -0.999999f)
			allignement = glm::angleAxis(glm::pi<float>(), glm::vec3(1, 0, 0));
		else {
			glm::vec3	cross = glm::cross(constAllign->localUp, up);
			float		s = glm::sqrt((1.f + dot) * 2.f);
			allignement = glm::quat(s * 0.5f, cross.x / s, cross.y / s, cross.z / s);
		}
		constTransform.modify()->rotation = glm::normalize(allignement * constTransform->rotation);
		constAllign.modify()->localUp = allignement * constAllign->localUp;
	}
}

}
