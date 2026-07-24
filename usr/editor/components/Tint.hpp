/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Tint.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:07:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 15:26:50                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "core/ecs/IComponent.hpp"
#include <glm/glm.hpp>

namespace	hel::comp {

struct	Tint: IComponent<Tint> {
	struct	POD {
		glm::vec3	tint{1.f};
	};
	struct alignas(16)	GPULayout {
		glm::vec3	tint;
	};
	struct	MetaData: IComponent<Tint>::MetaData {
		static constexpr std::string_view	label = "Tint";
		static constexpr bool				gpuVisible = true;

		static GPULayout	toGPU(const POD &comp) {
			return {comp.tint};
		}
	};
};

}
