/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SelectionTag.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:32:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/03 11:33:20                                        */
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

struct	SelectedTag: IComponent<SelectedTag> {
	struct	MetaData: IComponent<SelectedTag>::MetaData {
		static constexpr std::string_view	label = "Selected Entity Tag";
	};
};

struct	NonSelectableTag: IComponent<NonSelectableTag> {
	struct	MetaData: IComponent<NonSelectableTag>::MetaData {
		static constexpr std::string_view	label = "Non Selectable Tag";
	};
};

}