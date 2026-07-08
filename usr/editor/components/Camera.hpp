/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:07:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/03 11:08:13                                        */
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

namespace	hel::comp {

struct	Camera: IComponent<Camera> {
	struct	POD {
		float		fov{70};
		float		near{0.1f};
		float		far{1000.f};

		glm::mat4	view{1.f};
	};
	struct	GPULayout {
		glm::mat4	viewMatrix;
	};
	struct	MetaData: IComponent<Camera>::MetaData {
		static constexpr std::string_view	label = "Camera";
		static constexpr bool				gpuVisible = true;

		static GPULayout	toGPU(const POD &comp) {
			return {comp.view};
		}
	};
};

}
