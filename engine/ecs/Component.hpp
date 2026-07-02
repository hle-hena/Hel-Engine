/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Component.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/21 11:31:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/02 15:13:27                                        */
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
#include "ecs/IComponent.hpp"

namespace	hel::comp {

struct	Hierarchy: IComponent<Hierarchy> {
	struct	POD {
		Entity::id				parentId{Entity::NOT_REGISTERED};
		std::vector<Entity::id>	childrenId{};
	};
	struct	MetaData: IComponent<Hierarchy>::MetaData {
		static constexpr std::string_view	label = "Hierarchy";
	};
};

struct	EditorControllerTag: IComponent<EditorControllerTag> {
	struct	MetaData: IComponent<EditorControllerTag>::MetaData {
		static constexpr std::string_view	label = "Editor Controller Tag";
	};
};

struct	BaseControllerTag: IComponent<BaseControllerTag> {
	struct	MetaData: IComponent<BaseControllerTag>::MetaData {
		static constexpr std::string_view	label = "Base Controller Tag";
	};
};

struct	HideEntityTag: IComponent<HideEntityTag> {
	struct	MetaData: IComponent<HideEntityTag>::MetaData {
		static constexpr std::string_view	label = "Hide Entity Tag";
	};
};

struct	HideEntityInHierarchyTag: IComponent<HideEntityInHierarchyTag> {
	struct	MetaData: IComponent<HideEntityInHierarchyTag>::MetaData {
		static constexpr std::string_view	label = "Hide Entity In Hierarchy Tag";
	};
};

struct	NonSelectableTag: IComponent<NonSelectableTag> {
	struct	MetaData: IComponent<NonSelectableTag>::MetaData {
		static constexpr std::string_view	label = "Non Selectable Tag";
	};
};

struct	SelectedTag: IComponent<SelectedTag> {
	struct	MetaData: IComponent<SelectedTag>::MetaData {
		static constexpr std::string_view	label = "Selected Entity Tag";
	};
};

struct	Name: IComponent<Name> {
	struct	POD {
		std::string	name{"Default Entity Name"};
	};
	struct	MetaData: IComponent<Name>::MetaData {
		static constexpr std::string_view	label = "Entity Name";
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
	struct	MetaData {
		static constexpr std::string_view	label = "Transform";
		static constexpr bool				gpuVisible = true;

		static GPULayout	toGPU(const POD &comp) {
			return {comp.worldMatrix, comp.normalMatrix};
		}
	};
};

struct	Model: IComponent<Model> {
	struct	POD {
		std::string	modelName{""};
	};
	struct	MetaData: IComponent<Model>::MetaData {
		static constexpr std::string_view	label = "Model";
	};
};

struct	Texture: IComponent<Texture> {
	struct	POD {
		std::string	filepath{""};
	};
	struct	MetaData: IComponent<Texture>::MetaData {
		static constexpr std::string_view	label = "Texture";
	};
};

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
		static constexpr const char	*label = "Camera";
		static constexpr const bool	gpuVisible = true;

		static GPULayout	toGPU(const POD &comp) {
			return {comp.view};
		}
	};
};

struct	Controller: IComponent<Controller> {
	struct	POD {
		float	mouseSensivity{0.001f};
		float	movementSpeed{30.f};

		int		forwardKey{GLFW_KEY_W};
		int		backwardKey{GLFW_KEY_S};
		int		leftStrideKey{GLFW_KEY_A};
		int		rightStrideKey{GLFW_KEY_D};
		int		upKey{GLFW_KEY_SPACE};
		int		downKey{GLFW_KEY_LEFT_SHIFT};
	};
	struct	MetaData: IComponent<Camera>::MetaData {
		static constexpr const char	*label = "Controller";
	};
};

}
