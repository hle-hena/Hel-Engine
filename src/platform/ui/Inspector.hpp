/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Inspector.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/14 19:23:16 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/18 10:23:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <unordered_map>
# include <typeindex>
# include <functional>

# include "ecs/Entity.hpp"
# include "api/ImGui/imgui.h"
# include "platform/ui/Panel.hpp"

namespace	hel {

class	Registry;
class	Window;

}

namespace	hel::sys {

class	Inspector : public Panel<Inspector> {
	public:
		static constexpr const char	*label = "Inspector";
		using UIDrawFunc = std::function<void(Window *, void *)>;

		Inspector(void) = default;
		~Inspector(void) = default;

		expected<void, std::string>	onInit(void) override;

		template <typename Component>
		void	setDrawFunc(UIDrawFunc func) {
			_drawFuncs[typeid(Component)] = func;
		}

		void	render(Window *window, const ImVec2 &) override;

	private:
		void	addNewComponentPopup(Entity::id handle);
		void	removeEntity(Entity::id handle);

		void	setBuiltInDrawFunc(void);

		bool		_addNewComp{false};
		int			_newCompTypeIndex{0};

		std::unordered_map<std::type_index, UIDrawFunc>	_drawFuncs;
};

}
