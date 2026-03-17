/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Dock.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:30:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/16 13:10:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "platform/ui/Panel.hpp"
# include "api/ImGui/imgui.h"
# include <vector>

namespace	hel::sys {

class	Dock {
	public:
		enum class	Type { Split, TabGroup };

		Dock(std::initializer_list<IPanel *> panels)	: _panels{panels} {}

		void	render(Window *window);

	private:
		Type	_type{Type::TabGroup};

		std::vector<IPanel *>	_panels;
	
	friend class	IPanel;
};

}
