/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:27 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 15:31:27                                        */
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

		void	update(const FrameContext &ctx);
		void	postProcessing(const Renderer &renderer);
		void	renderInteraction(const Renderer &renderer);

	private:
		struct	EntityData {
			uint32_t	entityIndex{0};
			uint32_t	transformIndex{0};
		};

		static void	configurePipeline(PipelineConfig &config);

		AssetManager				*_assetManager;
		PipelineMap					*_tintPipeline{nullptr};
		Entity::id					_selectedEntity;

		std::unordered_map<RenderRequest, Read::Context, RenderRequest::Hasher>	_requests;
};

}
