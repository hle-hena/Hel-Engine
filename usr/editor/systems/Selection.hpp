/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:27 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/28 18:46:41                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "core/HelSystem.hpp"

namespace	hel::sys {

class	Selection : public ISystem {
	public:
		Selection(void) = default;
		~Selection(void) = default;

		void	init(void) override;

		void	update(const ExecutionContext &ctx);
		void	postProcessing(const Renderer &renderer);
		void	renderInteraction(const Renderer &renderer);

		static void			setSelected(Entity::id handle)
			{ _newSelected = handle; }
		static Entity::id	getSelected()
			{ return _selectedEntity; }

	private:
		struct	EntityData {
			uint32_t	entityIndex{0};
			uint32_t	transformIndex{0};
		};

		static void	configurePipeline(PipelineConfig &config);

		AssetManager						*_assetManager;
		PipelineMap							*_tintPipeline{nullptr};
		static Entity::id					_selectedEntity;
		static std::optional<Entity::id>	_newSelected;

		std::unordered_map<RenderRequest, Read::Context>	_requests;
};

}
