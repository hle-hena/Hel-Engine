/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/02 15:50:30                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "core/SystemManager.hpp"
#include <queue>

namespace	hel {

SystemManager::UnderlyingVec				SystemManager::_data{};
std::vector<sys::ISystem*>				SystemManager::_update{};
std::vector<sys::ISystem*>				SystemManager::_uInteraction{};
std::vector<std::vector<sys::ISystem*>>	SystemManager::_render{{}};
std::vector<std::vector<sys::ISystem*>>	SystemManager::_postProcess{{}};
std::vector<sys::ISystem*>				SystemManager::_rInteraction{};

#define depNotFound(depName, depStatus, parentProvide)		\
do {														\
	std::cerr << "Didn't find the dependance \"" << depName	\
		<< "\" listed as " << depStatus						\
		<< " for the system providing \"" << parentProvide	\
		<< "\"\n";											\
} while (0)

#define sortSystems(cycleDeps, cycleName)										\
do {																			\
	std::unordered_map<std::string, sys::ISystem *>					byName;		\
	std::unordered_map<sys::ISystem *, std::vector<sys::ISystem *>>	adj;		\
	std::unordered_map<sys::ISystem *, uint32_t>					inDegree;	\
																				\
	for (auto &system: _data) {													\
		if (system->cycleDeps.provides.has_value())	{							\
			byName[system->cycleDeps.provides.value()] = system.get();			\
			inDegree[system.get()] = 0;											\
		}																		\
	}																			\
																				\
	for (auto &[name, system]: byName) {										\
		for (auto &dep: system->cycleDeps.block) {								\
			auto it = byName.find(dep);											\
			if (it == byName.end()) {											\
				depNotFound(dep, "block", name); continue ;						\
			}																	\
			adj[system].push_back(it->second);									\
			inDegree[it->second]++;												\
		}																		\
		for (auto &dep: system->cycleDeps.require) {							\
			auto it = byName.find(dep);											\
			if (it == byName.end()) {											\
				depNotFound(dep, "require", name); continue ;					\
			}																	\
			adj[it->second].push_back(system);									\
			inDegree[system]++;													\
		}																		\
	}																			\
	std::queue<sys::ISystem *>	queue;											\
	for (auto &[system, degree]: inDegree)										\
		if (degree == 0)														\
			queue.push(system);													\
	while (!queue.empty()) {													\
		auto	current = queue.front();	queue.pop();						\
		cycleName.push_back(current);											\
																				\
		for (auto &neighbor: adj[current]) {									\
			if (--inDegree[neighbor] == 0)										\
				queue.push(neighbor);											\
		}																		\
	}																			\
																				\
	if (cycleName.size() != byName.size())										\
		std::cerr << "Loop detected in the cycle\n";							\
} while (0)

#define printSystemOrder(listName, listSystems, depName)						\
do {																			\
	std::cout << "Order for " << listName << " : --";							\
	for (auto &system: listSystems)												\
		std::cout << "[" << system->depName.provides.value() << "]--";			\
	std::cout << "\n\n";														\
} while (0)

#define splitPasses(listName, depName)											\
do {																			\
	auto	allSystems = listName[0];											\
	listName.clear();															\
	std::vector<sys::ISystem *>	newList;										\
	for (auto &system: allSystems) {											\
		auto it = std::find(system->depName.require.begin(),					\
						system->depName.require.end(), "newPass");				\
		if (it != system->depName.require.end() && !newList.empty()) {			\
			listName.emplace_back(newList);										\
			newList.clear();													\
		}																		\
		newList.push_back(system);												\
		it = std::find(system->depName.block.begin(),							\
						system->depName.block.end(), "newPass");				\
		if (it != system->depName.block.end()) {								\
			listName.emplace_back(newList);										\
			newList.clear();													\
		}																		\
	}																			\
	listName.emplace_back(newList);												\
	if (!newList.empty())														\
		listName.emplace_back();												\
} while (0)

void	SystemManager::sort(EngineKey) {
	sortSystems(updateDeps, _update);
	sortSystems(updateInterDeps, _uInteraction);
	sortSystems(renderDeps, _render[0]);
	sortSystems(postProcessDeps, _postProcess[0]);
	sortSystems(renderInterDeps, _rInteraction);

	printSystemOrder("update", _update, updateDeps);
	printSystemOrder("update interaction", _uInteraction, updateInterDeps);
	printSystemOrder("render", _render[0], renderDeps);
	printSystemOrder("post processing", _postProcess[0], postProcessDeps);
	printSystemOrder("render interaction", _rInteraction, renderInterDeps);

	splitPasses(_render, renderDeps);
	splitPasses(_postProcess, postProcessDeps);
}

}
