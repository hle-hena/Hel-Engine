/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Hierarchy.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:07:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 14:33:52                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "core/ecs/IComponent.hpp"

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

}
