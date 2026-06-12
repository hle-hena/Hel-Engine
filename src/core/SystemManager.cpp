/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/12 15:24:33                                        */
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

std::vector<sys::ISystem::Func*>		SystemManager::_updateCycle{};
std::vector<std::vector<
			sys::ISystem::Func*>>		SystemManager::_renderCycle{};

SystemManager::UnderlyingVec			SystemManager::_data{};
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
	std::unordered_map<std::string_view, sys::ISystem *>			byName;		\
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
	if (!newList.empty())														\
		listName.emplace_back(newList);											\
} while (0)

static inline void	kahnSort(const std::vector<SystemManager::SysPtr> &data,
			std::vector<sys::ISystem::Func*> &target) {
	using strView = std::string_view;
	std::unordered_map<strView, sys::ISystem::Func*>		byName;
	std::unordered_map<strView, std::vector<strView>>		adj;
	std::unordered_map<strView, uint32_t>					inDegree;

	for (auto &sys: data) {
		for (auto &[provide, func]: sys->updateCycleDep) {
			if (byName.contains(provide)) {
				std::cerr << "Duplicated provide \"" << provide << "\".\n";
				return ;
			}
			byName[provide] = &func;
			inDegree[provide] = 0;
		}
	}
	for (auto &[provide, systemFunc]: byName) {
		for (auto &require: systemFunc->getDep()->require) {
			if (!byName.contains(require))
				depNotFound(require, "require", provide);
			inDegree[provide]++;
			adj[require].push_back(provide);
		}
		for (auto &block: systemFunc->getDep()->block) {
			if (!byName.contains(block))
				depNotFound(block, "block", provide);
			inDegree[block]++;
			adj[provide].push_back(block);
		}
	}

	std::queue<strView>	queue;
	for (auto &[provide, degree]: inDegree)
		if (degree == 0)	queue.push(provide);
	while (!queue.empty()) {
		auto	current = queue.front();	queue.pop();
		target.push_back(byName[current]);
		for (auto &neighbor: adj[current]) {
			if (--inDegree[neighbor] == 0)
				queue.push(neighbor);
		}
	}
	if (target.size() != byName.size()) {
		std::cerr << "Loop detected in the cycle\n";
		target.clear();
	}
}

void	SystemManager::sort(EngineKey) {
	_updateCycle.clear();
	kahnSort(_data, _updateCycle);
	// _renderCycle.clear();
	// _renderCycle.push_back({});
	// kahnSort(_data, _renderCycle[0]);


	sortSystems(renderDeps, _render[0]);
	sortSystems(postProcessDeps, _postProcess[0]);
	sortSystems(renderInterDeps, _rInteraction);

	printSystemOrder("render", _render[0], renderDeps);
	printSystemOrder("post processing", _postProcess[0], postProcessDeps);
	printSystemOrder("render interaction", _rInteraction, renderInterDeps);

	splitPasses(_render, renderDeps);
	splitPasses(_postProcess, postProcessDeps);
}

}
