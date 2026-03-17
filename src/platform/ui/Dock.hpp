/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Dock.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:30:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/17 23:25:26                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "platform/ui/Panel.hpp"
# include "platform/ui/UIHelper.hpp"
# include <vector>
# include <memory>

namespace	hel::sys {

class	Dock {
	public:
		enum class	Type { Split, TabGroup };

		Dock(const std::string &dockName)	: _dockName{dockName} {}

		void	render(Window *window, ImVec2 size);

	private:
		Type		_type{Type::TabGroup};
		std::string	_dockName;

		void	split(Splitter::Dir dir, IPanel *splitPanel);
		void	merge(void);

		struct	RenderDragDropContext {
			ImVec2	topLeft, topRight, bottomLeft, bottomRight, center;
			ImVec2	origin;
			ImVec2	mouse;
			float	tabBarH;
			bool	released;

			RenderDragDropContext(void);
		};

		bool	renderTriangleZones(const RenderDragDropContext &ctx,
								ImDrawList *draw, IPanel *panel);
		void	renderTabBarZone(const RenderDragDropContext &ctx,
								ImDrawList *draw, IPanel *panel);
		void	renderDragDrop(void);
		void	renderPanels(Window *window);
		std::vector<IPanel *>	_panels{};

		void	renderSplits(Window *window, ImVec2 size);
		std::unique_ptr<Dock>	_childOne{nullptr};
		std::unique_ptr<Dock>	_childTwo{nullptr};
		float					_splitRatio{0.5f};
		Splitter::Dir			_splitDir;

	friend class	IPanel;
};

}
