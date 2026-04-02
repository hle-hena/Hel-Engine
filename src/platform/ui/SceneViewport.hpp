/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:39 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 18:02:26                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <ui/ImGui/imgui.h>
# include "platform/ui/Panel.hpp"
# include "ecs/Entity.hpp"

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
		bool		_captured;
		Entity::id	_handle{Entity::NOT_REGISTERED};
};

}
