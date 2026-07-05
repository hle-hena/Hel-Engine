/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 19:41:44                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <memory>
#include <functional>

#include "HelSystem.hpp"
#include "systems/ui/Dock.hpp"
#include "core/RenderQueue.hpp"

#define PanelFactoryMacro(panelType)							\
	[](UI *ui, Dock *dock){ ui->addNewPanel<panelType>(dock); }

namespace	hel {

class	Window;

}

namespace	hel::sys {

class	UI : public ISystem {
	public:
		using PanelFactory = std::function<void (UI *, Dock *)>;

		UI(void) = default;
		~UI(void);

		void	init(void) override;
		void	saveToFile(const std::string &path);
		bool	loadFromFile(const std::string &path);

		template <typename T>
		void	addNewPanel(Dock *dock);
		void	removePanel(IPanel *panel);

		void	addNewPanelRegistry(const std::string &panelName,
									PanelFactory factory)
					{ _panelRegistry.push_back({panelName, factory}); }
		const auto	&getPanelRegistry(void) const
					{ return (_panelRegistry); }

		void	updateUI(const FrameContext &ctx);
		void	render(const Renderer &renderer);

	private:
		void	addDock(Window *window, const ImVec2 &size);

		std::unique_ptr<Dock>	_dock;
		std::optional<ImVec2>	_lastSize;

		std::vector<std::unique_ptr<IPanel>>	_panels;
		RenderRequest							_request;

		std::vector<std::pair<
			std::string, PanelFactory>>			_panelRegistry;
};

}

# include "systems/ui/UI.tpp"
