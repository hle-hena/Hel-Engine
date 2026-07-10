/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Light.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/09 10:23:30 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/09 11:05:31                                        */
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

namespace	hel::comp {

struct	SpotLight: IComponent<SpotLight> {
	struct	POD {
		glm::vec4	color;
		glm::vec3	dir;
		glm::vec3	pos;
		float		beamAngle;
	};
	struct alignas(16)	GPULayout {
		glm::vec4	color;
		glm::vec3	dir;
		float		beamAngle;
	};
	struct	MetaData {
		static constexpr std::string_view	label = "Spotlight";
		static constexpr bool				gpuVisible = true;

		static GPULayout	toGPU(const POD &comp) {
			return {comp.color, comp.dir, comp.beamAngle};
		}
	};
};

}
