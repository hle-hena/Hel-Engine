/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:39 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/18 10:58:53                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "api/ImGui/imgui.h"
# include "platform/ui/Panel.hpp"

namespace	hel {

class	Window;
class	Device;
class	ImagePool;

}

namespace	hel::sys {

class	SceneViewport : public Panel<SceneViewport> {
	public:
		static constexpr const char	*label = "Viewport";
		SceneViewport(void) = default;
		~SceneViewport(void) = default;

		expected<void, std::string>	onInit(void) override;

		void	render(Window *window, const ImVec2 &size) override;

	private:
		bool	_captured;
};

}
