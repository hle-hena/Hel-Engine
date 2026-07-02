/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 17:09:00 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/30 18:30:24                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "HelComponent.hpp"
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

		// bool		isDirty{true};
	};
	struct	MetaData {
		static constexpr std::string_view	label = "Transform";
		static constexpr bool				gpuVisible = true;

		static GPULayout	toGPU(const POD &comp);
	};
};


// struct	TransformGPU {
// 	glm::mat4	worldMatrix;
// 	glm::mat4	normalMatrix;
// };

// struct	Transform {
// 	static constexpr const char	*label = "Transform";
// 	static constexpr const bool	gpuVisible = true;

// 	using GPUType = TransformGPU;
// 	TransformGPU	toGPU(void) {
// 		return {worldMatrix, normalMatrix};
// 	}

// 	glm::vec3	position{0.f};
// 	glm::quat	rotation{1.f, 0.f, 0.f, 0.f};
// 	glm::vec3	scale{1.f};

// 	glm::mat4	worldMatrix{1.f};
// 	glm::mat4	normalMatrix{1.f};

// 	bool		isDirty{true};
// };

// struct	OffsetTransform {
// 	static constexpr const char	*label = "Offset Transform";

// 	glm::vec3	pos{0.f};
// 	glm::quat	rotation{1.f, 0.f, 0.f, 0.f};
// 	glm::vec3	scale{1.f};
// };

}
