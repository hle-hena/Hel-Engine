/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Dock.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:30:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/18 13:56:41                                        */
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

class	UI;

class	Dock {
	public:
		enum class	Type { Split, TabGroup };

		Dock(const std::string &dockName)	: _dockName{dockName} {}

		void	render(Window *window, const ImVec2 &size);

		PASSKEY(UIKey, UI)
		void	forceSplit(Splitter::Dir dir, IPanel *splitPanel, UIKey)
					{ split(dir, splitPanel); }
		Dock	*forceGetChildOne(UIKey)
					{ return (_childOne.get()); }
		Dock	*forceGetChildTwo(UIKey)
					{ return (_childTwo.get()); }

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

			RenderDragDropContext(const ImVec2 &size);
		};

		bool	renderTriangleZones(const RenderDragDropContext &ctx,
								ImDrawList *draw, IPanel *panel);
		void	renderTabBarZone(const RenderDragDropContext &ctx,
								ImDrawList *draw, IPanel *panel);
		void	renderDragDrop(const ImVec2 &size);
		void	renderPanels(Window *window, const ImVec2 &size);
		std::vector<IPanel *>	_panels{};

		void	renderSplits(Window *window, const ImVec2 &size);
		std::unique_ptr<Dock>	_childOne{nullptr};
		std::unique_ptr<Dock>	_childTwo{nullptr};
		std::optional<float>	_splitRatio;
		Splitter::Dir			_splitDir;
		bool					_askForMerge{false};

	friend class	IPanel;
};

}
