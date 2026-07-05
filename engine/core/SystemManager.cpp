/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SystemManager.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/05/29 16:22:26 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/05 16:29:03                                        */
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

std::vector<sys::CycleEntry*>		SystemManager::_updateCycle{};
std::unordered_map<std::string_view,
	std::vector<std::vector<
			sys::CycleEntry*>>>		SystemManager::_renderCycle{};

std::vector<SystemManager::SysPtr>		SystemManager::_data{};

bool	matchName(const std::string_view &pattern,
				const std::string_view &name)
{
	if (pattern.ends_with('*'))
		return name.starts_with(pattern.substr(0, pattern.size() - 1));
	return pattern == name;
};

#define	RETURN_ERROR(functionProvide, imageName, error)	\
do {													\
	std::cerr << "Error for the function providing \""	\
		<< functionProvide << "\", on the image \""		\
		<< imageName << "\"." << error;					\
	return true;										\
} while (0)

struct	LayerState {
	std::unordered_map<uint32_t, std::string_view>	colors;
	std::optional<std::string_view>					depth;
	std::optional<std::string_view>					stencil;
	std::vector<std::string_view>					reads;

	bool	validate(const std::string_view &functionProvide,
					const ImageDep &write) {
		if (write._format == VK_FORMAT_MAX_ENUM)
			RETURN_ERROR(functionProvide, write._imageName,
				"Invalid format asked.\n");

		if ((write._usage & ImageDep::Color)
			&& (write._usage & ImageDep::DepthStencil))
			RETURN_ERROR(functionProvide, write._imageName,
					"Inconsistent usage. Cannot mix Color and Depth/Stencil.");
		return false;
	}

	bool	check(const ImageDep &write) {
		if (std::any_of(reads.begin(), reads.end(),
			[&](const auto &value){return matchName(value, write._imageName);}))
			return true;
		if (write._usage & ImageDep::Color
				&& colors.contains(write._bindingIndex)
				&& colors[write._bindingIndex] != write._imageName)
			return true;
		if (write._usage & ImageDep::Depth
				&& depth.has_value()
				&& depth.value() != write._imageName)
			return true;
		if (write._usage & ImageDep::Stencil
				&& stencil.has_value()
				&& stencil.value() != write._imageName)
			return true;
		if (write._usage & ImageDep::Color &&
			((depth.has_value() && depth == write._imageName)
			|| (stencil.has_value() && stencil == write._imageName)))
			return true;
		if (write._usage & ImageDep::DepthStencil &&
			std::any_of(colors.begin(), colors.end(), [&](const auto &p)
				{ return p.second == write._imageName; }))
			return true;
		track(write);
		return false;
	}
	bool	check(std::string_view read) {
		if (depth.has_value() && matchName(read, depth.value()))
			return true;
		if (stencil.has_value() && matchName(read, stencil.value()))
			return true;
		if (std::any_of(colors.begin(), colors.end(),
			[&](const auto &pair){return matchName(read, pair.second);}))
			return true;
		track(read);
		return false;
	}

	void	track(const ImageDep &write) {
		if (write._usage & ImageDep::Color)
			colors[write._bindingIndex] = write._imageName;
		if (write._usage & ImageDep::Depth)
			depth = write._imageName;
		if (write._usage & ImageDep::Stencil)
			stencil = write._imageName;
	}
	void	track(const std::string_view &read) {
		reads.push_back(read);
	}
};


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
	std::unordered_map<strView, sys::CycleEntry*>			byName;				\
	std::unordered_map<strView, std::vector<strView>>		adj;				\
	std::unordered_map<strView, uint32_t>					inDegree;			\
																				\
	for (auto &sys: data) {														\
		for (auto &[provide, func]: sys->depName) {								\
			if (byName.contains(provide)) {										\
				std::cerr << "Duplicated provide \"" << provide << "\".\n";		\
				continue ;														\
			}																	\
			auto	*dep = func.getDep();										\
			LayerState	state;													\
			bool	writeConflict = std::any_of(dep->write.begin(),				\
												dep->write.end(),				\
												[&](const auto &write){			\
				return state.check(write) || state.validate(provide, write);});	\
			bool	readConflict = std::any_of(dep->read.begin(),				\
												dep->read.end(),				\
												[&](const auto &read){			\
													return state.check(read);});\
			if (writeConflict || readConflict) {								\
				std::cerr << "Error with some images, disregarding the function"\
					<< " providing \"" << provide << "\".\n";					\
				continue ;														\
			}																	\
			byName[provide] = &func;											\
			inDegree[provide] = 0;												\
		}																		\
	}																			\
	for (auto &[provide, systemFunc]: byName) {									\
		for (auto &require: systemFunc->getDep()->require) {					\
			if (!byName.contains(require)) {									\
				depNotFound(require, "require", provide);						\
				continue ;														\
			}																	\
			inDegree[provide]++;												\
			adj[require].push_back(provide);									\
		}																		\
		for (auto &block: systemFunc->getDep()->block) {						\
			if (!byName.contains(block)) {										\
				depNotFound(block, "block", provide);							\
				continue ;														\
			}																	\
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

void	SystemManager::sort(EngineKey) {
	_updateCycle.clear();
	kahnSort(_data, _updateCycle, updateCycleDep); std::cout << std::endl;
	_renderCycle.clear();
	FuncVec	sortedFuncs{};
	kahnSort(_data, sortedFuncs, renderCycleDep); std::cout << std::endl;
	splitPasses(sortedFuncs);
}

void	SystemManager::splitPasses(const FuncVec &sortedFuncs) {
	std::unordered_map<std::string_view, FuncVec>		newList;
	std::unordered_map<std::string_view, LayerState>	states;
	auto	addList = [&]() {
		for (auto &[layer, vec]: newList)
			_renderCycle[layer].emplace_back(vec);
		newList.clear(); states.clear();
	};
	auto	flushLayer = [&](std::string_view layer) {
		_renderCycle[layer].emplace_back(newList[layer]);
		newList.erase(layer); states.erase(layer);
	};

	for (auto func: sortedFuncs) {
		auto	dep = func->getDep();
		if (std::find(dep->require.begin(), dep->require.end(), "newPass")
				!= dep->require.end())
			addList();

		for (auto &layer: dep->layers) {
			bool	conflict = std::any_of(dep->write.begin(), dep->write.end(),
						[&](const ImageDep &w){return states[layer].check(w);});
			conflict |= std::any_of(dep->read.begin(), dep->read.end(),
						[&](const auto &r){return states[layer].check(r);});
			if (conflict)
				flushLayer(layer);
			newList[layer].push_back(func);
		}

		if (std::find(dep->block.begin(), dep->block.end(), "newPass")
				!= dep->block.end())
			addList();
	}
	if (!newList.empty())
		addList();
}

}
