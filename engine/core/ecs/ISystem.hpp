/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 14:44:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/08/01 16:03:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <utils/json.hpp>

#include "utils/Expected.hpp"

#include "core/scheduler/CycleEntry.hpp"
#include "rhi/render/PipelineMap.hpp"

namespace	hel {

class	Device;
class	Registry;
class	ImagePool;
struct	DrawCall;
class	Renderer;
class	InputState;

}

namespace	hel::sys {

class	ISystem {
	public:
		ISystem(void) = default;
		virtual ~ISystem(void) = default;

		ISystem(const ISystem &other) = delete;
		ISystem	&operator=(const ISystem &other) = delete;

		virtual void	init(Device *device, Registry *registry,
							ImagePool *imagePool, InputState *input) final;

		using UpdateFn = void (ISystem::*)(const ExecutionContext &);
		using RenderFn = void (ISystem::*)(const Renderer &);
		std::unordered_map<std::string_view, CycleEntry>	updateCycleDep;
		std::unordered_map<std::string_view, CycleEntry>	renderCycleDep;

	protected:
		virtual void	init(void) = 0;

		template <typename T>
		CycleEntry	*addUpdateDep(std::string_view depName,
							void (T::*fn)(const ExecutionContext&)) {
			auto	[it, _] = updateCycleDep.emplace(depName,
								CycleEntry(this, static_cast<UpdateFn>(fn)));
			return &it->second;
		}
		template <typename T>
		CycleEntry	*addRenderDep(std::string_view depName,
							void (T::*fn)(const Renderer&)) {
			auto	[it, _] = renderCycleDep.emplace(depName,
								CycleEntry(this, static_cast<RenderFn>(fn)));
			return &it->second;
		}
		template <typename T>
		CycleEntry	createRenderFunc(void (T::*fn)(const Renderer&)) {
			return {this, static_cast<RenderFn>(fn)};
		}

		virtual PipelineMap	*createPipeline(const
								PipelineMap::Config &config) final;

		virtual DrawCall	drawCommand(const Renderer &renderer,
											PipelineMap *pipeline) const final;

		Device										*_device;
		Registry									*_registry;
		InputState									*_inputState;
		ImagePool									*_imagePool;

	private:
		expected<void>		validateCycleEntry(nlohmann::json &entry,
												std::string_view entryType,
												size_t entryIndex) const;
		expected<void>		validateSystem(nlohmann::json &sys) const;
		expected<std::optional<size_t>>	findSystemIndex(const nlohmann::json &src) const;

		expected<void>	loadCycleEntry(std::string_view jsonFilepath,
							const std::string &systemName);
		void			saveCycleEntry(void) const;

		nlohmann::json	serialize(void) const;
		void			deserialize(const nlohmann::json &src);

		std::string	_systemName;
		std::string	_jsonFilepath;

		std::vector<std::unique_ptr<PipelineMap>>	_pipelines;

	friend struct SystemManager;
};

}
