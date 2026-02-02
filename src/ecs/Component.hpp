/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Component.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 11:31:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 13:47:29                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# define GLM_FORCE_RADIANS
# define GLM_FORCE_DEPTH_ZERO_TO_ONE
# include <glm/glm.hpp>
# include <glm/gtc/quaternion.hpp>
# include <glm/gtc/constants.hpp>
# include <string>

namespace	hel {

struct	Name {
	std::string	name;
};

struct	Transform {
	glm::vec3	position{0.f};
	glm::quat	rotation{1.f, 0.f, 0.f, 0.f};
	glm::vec3	scale{1.f};

	glm::mat4	worldMatrix{1.f};

	bool		isDirty{true};
};

struct	Camera {
	float		fov{glm::radians<float>(70)};
	float		aspect{1.f};
	float		near{0.1f};
	float		far{1000.f};
	glm::vec3	up{0.f, 1.f, 0.f};
	glm::vec3	direction{0.f, 0.f, 0.f};
	glm::mat4	viewProjection{1.f};

	bool		isDirty{true};
};


}
