/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Component.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 11:31:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/21 20:47:57                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <glm/ext/matrix_float4x4.hpp>
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

struct	HideEntityTag {
	static constexpr const char	*label = "Hide Entity Tag";
};

struct	HideEntityInHierarchyTag {
	static constexpr const char	*label = "Hide Entity Tag";
};

struct	NonSelectableTag {
	static constexpr const char	*label = "Non selectable Tag";
};

struct	SelectedTag {
	static constexpr const char	*label = "Hide Entity Tag";
};

struct	Name {
	static constexpr const char	*label = "Entity Name";

	std::string	name{"Paul"};
};

struct	TransformGPU {
	glm::mat4	worldMatrix;
	glm::mat4	normalMatrix;
};

struct	Transform {
	static constexpr const char	*label = "Transform";
	static constexpr const bool	gpuVisible = true;

	using GPUType = TransformGPU;
	TransformGPU	toGPU(void) {
		return {worldMatrix, normalMatrix};
	}

	glm::vec3	position{0.f};
	glm::quat	rotation{1.f, 0.f, 0.f, 0.f};
	glm::vec3	scale{1.f};

	glm::mat4	worldMatrix{1.f};
	glm::mat4	normalMatrix{1.f};

	bool		isDirty{true};
};

struct	OffsetTransform {
	static constexpr const char	*label = "Offset Transform";

	glm::vec3	pos{0.f};
	glm::quat	rotation{1.f, 0.f, 0.f, 0.f};
	glm::vec3	scale{1.f};
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

struct	Texture {
	static constexpr const char	*label = "Texture";

	std::string	filePath{""};
};

struct alignas(16)	TintGPU {
	glm::vec3	tint;
};

struct	Tint {
	static constexpr const char	*label = "Tint";
	static constexpr const bool	gpuVisible = true;

	using GPUType = TintGPU;
	TintGPU	toGPU(void) {
		return {tint};
	}

	glm::vec3	tint{1.f};
};

struct	CameraGPU {
	glm::mat4	viewMatrix;
};

struct	Camera {
	static constexpr const char	*label = "Camera";
	static constexpr const bool	gpuVisible = true;

	using GPUType = CameraGPU;
	CameraGPU	toGPU(void) {
		return {view};
	}

	float		fov{70};
	float		near{0.1f};
	float		far{1000.f};

	glm::mat4	view{1.f};

	bool		isDirty{true};
};

struct	Controller {
	static constexpr const char	*label = "Controller";

	float	mouseSensivity{0.001f};
	float	movementSpeed{30.f};

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
