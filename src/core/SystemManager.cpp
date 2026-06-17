/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/17 13:53:44                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "core/SystemManager.hpp"
#include <queue>
#include <iostream>

namespace	hel {

std::vector<sys::ISystem::Func*>		SystemManager::_updateCycle{};
std::unordered_map<std::string_view,
	std::vector<std::vector<
			sys::ISystem::Func*>>>		SystemManager::_renderCycle{};

std::vector<SystemManager::SysPtr>		SystemManager::_data{};

#define depNotFound(depName, depStatus, parentProvide)		\
do {														\
	std::cerr << "Didn't find the dependance \"" << depName	\
		<< "\" listed as " << depStatus						\
		<< " for the system providing \"" << parentProvide	\
		<< "\"\n";											\
} while (0)

#define kahnSort(data, target, depName)											\
do {																			\
	using strView = std::string_view;											\
	std::unordered_map<strView, sys::ISystem::Func*>		byName;				\
	std::unordered_map<strView, std::vector<strView>>		adj;				\
	std::unordered_map<strView, uint32_t>					inDegree;			\
																				\
	for (auto &sys: data) {														\
		for (auto &[provide, func]: sys->depName) {								\
			if (byName.contains(provide)) {										\
				std::cerr << "Duplicated provide \"" << provide << "\".\n";		\
				return ;														\
			}																	\
			byName[provide] = &func;											\
			inDegree[provide] = 0;												\
		}																		\
	}																			\
	for (auto &[provide, systemFunc]: byName) {									\
		for (auto &require: systemFunc->getDep()->require) {					\
			if (!byName.contains(require))										\
				depNotFound(require, "require", provide);						\
			inDegree[provide]++;												\
			adj[require].push_back(provide);									\
		}																		\
		for (auto &block: systemFunc->getDep()->block) {						\
			if (!byName.contains(block))										\
				depNotFound(block, "block", provide);							\
			inDegree[block]++;													\
			adj[provide].push_back(block);										\
		}																		\
	}																			\
																				\
	std::queue<strView>	queue;													\
	for (auto &[provide, degree]: inDegree)										\
		if (degree == 0)	queue.push(provide);								\
	while (!queue.empty()) {													\
		auto	current = queue.front();	queue.pop();						\
		target.push_back(byName[current]);										\
		std::cout << " -[" << current << "]-";									\
		for (auto &neighbor: adj[current]) {									\
			if (--inDegree[neighbor] == 0)										\
				queue.push(neighbor);											\
		}																		\
	}																			\
	if (target.size() != byName.size()) {										\
		std::cerr << "Loop detected in the cycle\n";							\
		target.clear();															\
	}																			\
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

void	SystemManager::sort(EngineKey) {
	_updateCycle.clear();
	kahnSort(_data, _updateCycle, updateCycleDep); std::cout << std::endl;
	_renderCycle.clear();
	FuncVec	sortedFuncs{};
	kahnSort(_data, sortedFuncs, renderCycleDep); std::cout << std::endl;

	_renderCycle.clear();
	std::unordered_map<std::string_view,
		FuncVec>	newList;
	auto	addList = [&](){
		for (auto &[layer, vec]: newList)
			_renderCycle[layer].emplace_back(vec);
	};
	for (auto func: sortedFuncs) {
		auto	dep = func->getDep();
		if (std::find(dep->require.begin(), dep->require.end(), "newPass")
				!= dep->require.end())
		{
			addList();
			newList.clear();
		}
		for (auto &layer: dep->layers)
			newList[layer].push_back(func);
		if (std::find(dep->block.begin(), dep->block.end(), "newPass")
				!= dep->block.end())
		{
			addList();
			newList.clear();
		}
	}
	if (!newList.empty())
		addList();
}

}
