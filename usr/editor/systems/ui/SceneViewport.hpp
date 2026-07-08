/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:39 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/06 10:39:18                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <ui/ImGui/imgui.h>
#include <string>

#include "systems/ui/Panel.hpp"
#include "ecs/Entity.hpp"

namespace	hel {

class	Window;
struct	RenderRequest;

}

namespace	hel::sys {

class	SceneViewport : public Panel<SceneViewport> {
	public:
		static constexpr const char	*label = "Viewport";
		SceneViewport(void) = default;
		~SceneViewport(void) = default;

		expected<void>	onInit(void) override;

		void	render(Window *window, const ImVec2 &size) override;

		RenderRequest	*mainRequest{nullptr};

	private:
		bool		_captured;
		Entity::id	_handle{Entity::NOT_REGISTERED};
		std::string	_showImage{"Color Image"};
};

}
