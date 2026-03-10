/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:39 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/10 16:28:14                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "api/ImGui/imgui.h"

namespace	hel {

class	Window;
class	Device;

}

namespace	hel::sys {

class	SceneViewport {
	public:
		SceneViewport(void) = default;
		~SceneViewport(void) = default;

		void	render(Window *window, ImVec2 pos, ImVec2 size);

	private:
};

}
