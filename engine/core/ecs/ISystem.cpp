/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.cpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/26 18:12:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/08/01 16:03:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/ecs/ISystem.hpp"
#include "core/Frame.hpp"
#include "rhi/render/Renderer.hpp"

#include <utils/json.hpp>
#include <fstream>

namespace	hel::sys {

void	ISystem::init(Device *device, Registry *registry,
					ImagePool *imagePool, InputState *input)
{
	_device = device;
	_registry = registry;
	_imagePool = imagePool;
	_inputState = input;
	init();
}

expected<void>	ISystem::loadCycleEntry(std::string_view jsonFilepath,
					const std::string &systemName) {
	std::cout << "Loading: " << jsonFilepath << "\n" << "System: "
		<< systemName << "\n\n";

	_systemName = systemName;
	_jsonFilepath = jsonFilepath;

	std::ifstream	file(_jsonFilepath);
	if (!file.is_open())
		return unexpected("Failed to open the file ({}).", jsonFilepath);

	nlohmann::json	src;
	file >> src;
	auto	idx = findSystemIndex(src);
	if (!idx)
		return unexpected("Cannot load the cycle entries: {}", idx.error());
	if (!*idx) {
		HEL_WARN("Couldn't find an entry for {} in {}. Defaulting to an empty "
			"entry.", _systemName, _jsonFilepath);
		return {};
	}
	if (auto res = validateSystem(src[**idx]); !res)
		return unexpected("Cannot load the cycle entries: {}", res.error());

	deserialize(src[**idx]);
	return {};
}

void	ISystem::saveCycleEntry(void) const {
	std::ifstream	inFile(_jsonFilepath);
	if (!inFile.is_open()) {
		HEL_ERROR("Failed to open the file ({}).", _jsonFilepath);
		return ;
	}

	nlohmann::json	src;
	inFile >> src;
	auto	idx = findSystemIndex(src);
	if (!idx) {
		HEL_ERROR("Cannot save the cycle entries: {}", idx.error());
		return ;
	}

	if (!*idx)
		src.push_back(serialize());
	else
		src[**idx] = serialize();

	std::ofstream	outFile(_jsonFilepath);
	if (!outFile.is_open()) {
		HEL_ERROR("Failed to open the file ({}).", _jsonFilepath);
		return ;
	}
	outFile << src.dump(2);
}

expected<void>	ISystem::validateCycleEntry(nlohmann::json &entry,
											std::string_view entryType,
											size_t entryIndex) const
{
	if (!entry.is_object())
		return unexpected("{}: The {} entry number {} for the {} system is not "
				"a JSON object.", _jsonFilepath, entryType, entryIndex,
				_systemName);

	struct	Field {
		std::string_view		name;
		nlohmann::json::value_t	type;
		std::string_view		type_str;

		bool	skippable;
	};
	auto	string_t = nlohmann::json::value_t::string;
	auto	array_t = nlohmann::json::value_t::array;
	static const std::vector<Field>	fields = {
		{"entry_name", string_t, "string", false},
		{"require_entry", array_t, "array", true},
		{"block_entry", array_t, "array", true},
		{"active_layer", array_t, "array", true}
	};

	for (const auto &field: fields) {
		if (!entry.contains(field.name)) {
			if (field.skippable) {
				HEL_WARN("{}: The {} entry number {} for the {} system is "
					"missing a required property: \"{}\". Defaulting to an "
					"empty one.",  _jsonFilepath, entryType, entryIndex,
					_systemName, field.name);
				entry[field.name] = nlohmann::json::array();
				continue ;
			}
			return unexpected("{}: The {} entry number {} for the {} system is "
					"missing a required property: \"{}\"", _jsonFilepath,
					entryType, entryIndex, _systemName, field.name);
		}
		if (entry[field.name].type() != field.type)
			return unexpected("{}: The {} entry number {} for the {} system has"
					" an invalid type for property \"{}\". Expected a(n) {}.",
					_jsonFilepath, entryType, entryIndex, _systemName,
					field.name, field.type_str);
	}

	for (const std::string_view fieldName:
			{"require_entry", "block_entry", "active_layer"})
	{
		if (!entry.contains(fieldName))
			continue ;
		for (const auto &item : entry[fieldName]) {
			if (!item.is_string())
				return unexpected("{}: The {} entry number {} for the {} system"
					" has a non-string value in the array \"{}\"",
					_jsonFilepath, entryType, entryIndex,
					_systemName, fieldName);
		}
	}
	return {};
}

expected<void>	ISystem::validateSystem(nlohmann::json &sys) const {
	using	nameType_pair = std::pair<std::string_view, std::string_view>;
	static const std::vector<nameType_pair>	cycles = {
		{"render_entries", "render"},
		{"update_entries", "update"}
	};
	for (const auto &[field, entryType]: cycles) {
		if (!sys.contains(field)) {
			HEL_WARN("{}: System \"{}\" is missing a {} array. Defaulting to an"
					" empty one.", _jsonFilepath, _systemName, field);
			sys[field] = nlohmann::json::array();
			continue ;
		}
		if (!sys[field].is_array())
			return unexpected("{}: System \"{}\"'s {} isn't the expected "
					"type. Expected an array.", _jsonFilepath,
					_systemName, field);
		for (size_t i = 0; i < sys[field].size(); ++i) {
			auto	res = validateCycleEntry(sys[field][i], entryType, i);
			if (!res)
				return unexpected(res.error());
		}
	}
	return {};
}

expected<std::optional<size_t>>	ISystem::findSystemIndex(const nlohmann::json &src) const {
	if (!src.is_array())
		return unexpected("{}: The base of the configuration should be a JSON "
				"array.", _jsonFilepath);

	for (size_t idx = 0; idx < src.size(); ++idx) {
		const auto	&sys = src[idx];

		if (!sys.is_object() || !sys.contains("name")
			|| !sys["name"].is_string())
		{
			return unexpected("{}: System entry at index {} must be a valid "
					"JSON object containing a string \"name\".",
					_jsonFilepath, idx);
		}

		if (sys["name"] == _systemName)
			return idx;
	}
	return std::nullopt;
}

nlohmann::json	ISystem::serialize(void) const {
	nlohmann::json	j = nlohmann::json();
	j["name"] = _systemName;
	j["render_entries"] = nlohmann::json::array();
	j["update_entries"] = nlohmann::json::array();

	for (auto &[entryName, entry]: renderCycleDep) {
		j["render_entries"].push_back({
			{"entry_name",		entryName},
			{"require_entry",	entry._ord.require},
			{"block_entry",		entry._ord.block},
			{"active_layer",	entry._ord.layers}
		});
	}
	for (auto &[entryName, entry]: updateCycleDep) {
		j["update_entries"].push_back({
			{"entry_name",		entryName},
			{"require_entry",	entry._ord.require},
			{"block_entry",		entry._ord.block},
			{"active_layer",	entry._ord.layers}
		});
	}

	return j;
}

void	ISystem::deserialize(const nlohmann::json &system) {
	for (auto &entry: system["render_entries"]) {
		std::string	entryName = entry["entry_name"];

		renderCycleDep.erase(entryName);
		for (std::string require: entry["require_entry"])
			renderCycleDep[entryName]._ord.require.push_back(std::move(require));
		for (std::string block: entry["block_entry"])
			renderCycleDep[entryName]._ord.block.push_back(block);
		for (std::string layer: entry["active_layer"])
			renderCycleDep[entryName]._ord.layers.push_back(layer);
	}
	for (auto &entry: system["update_entries"]) {
		std::string	entryName = entry["entry_name"];

		updateCycleDep.erase(entryName);
		for (std::string require: entry["require_entry"])
			updateCycleDep[entryName]._ord.require.push_back(std::move(require));
		for (std::string block: entry["block_entry"])
			updateCycleDep[entryName]._ord.block.push_back(block);
		for (std::string layer: entry["active_layer"])
			updateCycleDep[entryName]._ord.layers.push_back(layer);
	}
}





PipelineMap	*ISystem::createPipeline(const PipelineMap::Config &config) {
	auto	pipeline = std::unique_ptr<PipelineMap>(new PipelineMap(config));
	pipeline->initDefaultSets({Frame::getGlobalLayout()});
	auto	pipelinePtr = pipeline.get();
	_pipelines.push_back(std::move(pipeline));
	return (pipelinePtr);
}

DrawCall	ISystem::drawCommand(const Renderer &renderer,
									PipelineMap *pipeline) const {
	return (renderer.drawCommand(pipeline, {}));
}



}
