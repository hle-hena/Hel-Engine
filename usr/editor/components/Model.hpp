/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Model.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:07:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/03 11:08:41                                        */
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

struct	Model: IComponent<Model> {
	struct	POD {
		std::string	modelName{""};
	};
	struct	MetaData: IComponent<Model>::MetaData {
		static constexpr std::string_view	label = "Model";
	};
};

}
