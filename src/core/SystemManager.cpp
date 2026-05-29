/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/05/29 17:29:34                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "core/SystemManager.hpp"

namespace	hel {

SystemManager::UnderlyingVec				SystemManager::_data{};
std::vector<sys::ISystem*>				SystemManager::_update{};
std::vector<sys::ISystem*>				SystemManager::_uInteraction{};
std::vector<std::vector<sys::ISystem*>>	SystemManager::_render{{}};
std::vector<std::vector<sys::ISystem*>>	SystemManager::_postProcess{{}};
std::vector<sys::ISystem*>				SystemManager::_rInteraction{};

void	SystemManager::sort(EngineKey) {
std::cout << "_update" << _update.size()
	<< "\n_uInteraction" << _uInteraction.size()
	<< "\n_render" << _render[0].size()
	<< "\n_postProcess" << _postProcess[0].size()
	<< "\n_rInteraction" << _rInteraction.size() << std::endl;
}

}
