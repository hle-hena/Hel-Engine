/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.tpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/19 10:55:26 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/19 10:58:30                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/UI.hpp"

namespace	hel::sys {

template <typename T>
void	UI::addNewPanel(Dock *dock) {
	_panels.emplace_back(std::make_unique<T>());
	auto	panel = _panels.back().get();
	panel->setup(_registry, _imagePool);
	panel->changeOwner(dock);
}

}
