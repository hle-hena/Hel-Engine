/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:07:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/03 11:30:57                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "ecs/IComponent.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace	hel::comp {

struct	Transform: IComponent<Transform> {
	struct	POD {
		glm::vec3	position{0.f};
		glm::quat	rotation{1.f, 0.f, 0.f, 0.f};
		glm::vec3	scale{1.f};

		glm::mat4	worldMatrix{1.f};
		glm::mat4	normalMatrix{1.f};
	};
	struct	GPULayout {
		glm::mat4	worldMatrix;
		glm::mat4	normalMatrix;
	};
	struct	MetaData: IComponent<Transform>::MetaData {
		static constexpr std::string_view	label = "Transform";
		static constexpr bool				gpuVisible = true;

		static GPULayout	toGPU(const POD &comp) {
			return {comp.worldMatrix, comp.normalMatrix};
		}
	};
};

struct	OffsetTransform: IComponent<OffsetTransform> {
	struct	POD {
		glm::vec3	pos{0.f};
		glm::quat	rotation{1.f, 0.f, 0.f, 0.f};
		glm::vec3	scale{1.f};
	};
	struct	MetaData: IComponent<OffsetTransform>::MetaData {
		static constexpr std::string_view	label = "Offset Transform";
	};
};

struct	SurfaceAllignement: IComponent<SurfaceAllignement> {
	struct	POD {
		glm::vec3	localUp{0., 1., 0.};
		bool		isDynamic{false};
	};
	struct	MetaData: IComponent<SurfaceAllignement>::MetaData {
		static constexpr std::string_view	label = "Surface Allignement";
	};
};

}
