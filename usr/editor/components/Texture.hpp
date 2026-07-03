/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Texture.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:07:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/03 11:10:40                                        */
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

struct	Texture: IComponent<Texture> {
	struct	POD {
		std::string	filepath{""};
	};
	struct	MetaData: IComponent<Texture>::MetaData {
		static constexpr std::string_view	label = "Texture";
	};
};

}
