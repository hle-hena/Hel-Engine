/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HideTag.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:31:49 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/03 11:32:16                                        */
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

struct	HideEntityInHierarchyTag: IComponent<HideEntityInHierarchyTag> {
	struct	MetaData: IComponent<HideEntityInHierarchyTag>::MetaData {
		static constexpr std::string_view	label = "Hide Entity In Hierarchy Tag";
	};
};

struct	HideEntityTag: IComponent<HideEntityTag> {
	struct	MetaData: IComponent<HideEntityTag>::MetaData {
		static constexpr std::string_view	label = "Hide Entity Tag";
	};
};

}
