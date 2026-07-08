/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Controllers.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:26:42 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/03 11:27:51                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "ecs/IComponent.hpp"

namespace	hel::comp {

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
	struct	MetaData: IComponent<Controller>::MetaData {
		static constexpr std::string_view	label = "Controller";
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

}
