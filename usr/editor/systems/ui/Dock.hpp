/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Dock.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:30:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/29 14:19:48                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "systems/ui/Panel.hpp"
# include "systems/ui/UIHelper.hpp"

# include <utils/json.hpp>
# include <ui/ImGui/imgui.h>

# include <vector>
# include <memory>

namespace	hel {

struct	RenderRequest;

}

namespace	hel::sys {

class	UI;

class	Dock {
	public:
		enum class	Type { Split, TabGroup };

		Dock(const std::string &dockName, UI *ui)
				:	_ui{ui}, _dockName{dockName} {}

		void	render(RenderRequest *request, Window *window, const ImVec2 &size,
								const ImVec2 &rescale = {-1.f, -1.f});

		PASSKEY(UIKey, UI)
		std::pair<Dock *, Dock *>	forceSplit(Splitter::Dir dir,
												float splitRatio, UIKey);
		Dock	*forceGetChildOne(UIKey)
					{ return (_childOne.get()); }
		Dock	*forceGetChildTwo(UIKey)
					{ return (_childTwo.get()); }

		nlohmann::json					serialize(ImVec2 size) const;
		static std::unique_ptr<Dock>	deserialize(UI *ui,
													const nlohmann::json &src);

	private:
		UI			*_ui;
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
			ImU32	bgColor;
			ImU32	mainColor;

			RenderDragDropContext(const ImVec2 &size);
		};

		bool	renderTriangleZones(const RenderDragDropContext &ctx,
								ImDrawList *draw, IPanel *panel);
		void	renderTabBarZone(const RenderDragDropContext &ctx,
								ImDrawList *draw, IPanel *panel);
		void	renderDragDrop(const RenderDragDropContext &ctx);
		void	newPanelPopup(void);
		void	renderPanels(RenderRequest *request, Window *window, const ImVec2 &size);
		std::vector<IPanel *>	_panels{};
		std::vector<float>		_gaps{};

		void	renderSplits(RenderRequest *request, Window *window, const ImVec2 &size, const ImVec2 &rescale);
		std::unique_ptr<Dock>	_childOne{nullptr};
		std::unique_ptr<Dock>	_childTwo{nullptr};
		std::optional<float>	_splitRatio;
		Splitter::Dir			_splitDir{Splitter::Dir::Right};
		bool					_askForMerge{false};

	friend class	IPanel;
};

}
