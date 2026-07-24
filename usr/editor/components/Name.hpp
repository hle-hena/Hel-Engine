/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Name.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:07:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 15:26:48                                        */
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

struct	Name: IComponent<Name> {
	struct	POD {
		std::string	name{"Default Entity Name"};
	};
	struct	MetaData: IComponent<Name>::MetaData {
		static constexpr std::string_view	label = "Entity Name";
	};
};

}
