/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.hpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:34 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 19:57:38                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <memory>
# include <functional>

# include "ecs/systems/ISystem.hpp"

# include "platform/ui/Dock.hpp"

# define PanelFactoryMacro(panelType)							\
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

		void	registerUI(const FrameContext &ctx) override;

	private:
		void	addDock(Window *window, const ImVec2 &size);

		std::unique_ptr<Dock>	_dock;
		std::optional<ImVec2>	_lastSize;

		std::vector<std::unique_ptr<IPanel>>	_panels;

		std::vector<std::pair<
			std::string, PanelFactory>>			_panelRegistry;
};

}

# include "platform/ui/UI.tpp"
