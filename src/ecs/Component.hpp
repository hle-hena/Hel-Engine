/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Component.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 11:31:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 22:36:11                                        */
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
# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>

# include "ecs/Entity.hpp"

namespace	hel::comp {

struct	EditorControllerTag {
	static constexpr const char	*label = "Editor Controller Tag";
};

struct	BaseControllerTag {
	static constexpr const char	*label = "Base Controller Tag";
};

struct	Name {
	static constexpr const char	*label = "Entity Name";

	std::string	name;
};

struct	Transform {
	static constexpr const char	*label = "Transform";

	glm::vec3	position{0.f};
	glm::quat	rotation{1.f, 0.f, 0.f, 0.f};
	glm::vec3	scale{1.f};

	glm::mat4	worldMatrix{1.f};
	glm::mat4	normalMatrix{1.f};

	bool		isDirty{true};
};

struct	SurfaceAllignement {
	static constexpr const char	*label = "Surface Allignement";

	glm::vec3	localUp{0., 1., 0.};

	bool	isDynamic{false};
};

struct	Model {
	static constexpr const char	*label = "Model";

	std::string	filePath{""};
};

struct	Camera {
	static constexpr const char	*label = "Camera";

	float		fov{glm::radians<float>(70)};
	float		aspect{1.f};
	float		near{0.1f};
	float		far{100.f};
	glm::mat4	viewProjection{1.f};

	bool		isDirty{true};
};

struct	Controller {
	static constexpr const char	*label = "Controller";

	float	mouseSensivity{0.001f};
	float	movementSpeed{1.f};

	int		forwardKey{GLFW_KEY_W};
	int		backwardKey{GLFW_KEY_S};
	int		leftStrideKey{GLFW_KEY_A};
	int		rightStrideKey{GLFW_KEY_D};
	int		upKey{GLFW_KEY_SPACE};
	int		downKey{GLFW_KEY_LEFT_SHIFT};
};

struct	Hierarchy {
	static constexpr const char	*label = "Hierarchy";

	Entity::id				parentId{Entity::NOT_REGISTERED};
	std::vector<Entity::id>	childrenId{};
};


}
