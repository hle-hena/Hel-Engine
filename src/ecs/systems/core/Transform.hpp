/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/08 16:11:31                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "api/vulkan/PipelineMap.hpp"
#include "ecs/systems/ISystem.hpp"
#include <cstdint>
#include <vector>

namespace	hel {

class	AssetManager;

}

namespace	hel::sys {

class	Transform : public ISystem {
	public:
		enum class	Action {
			Move,
			Scale,
			Rotate
		};

		Transform(void) = default;
		~Transform(void) = default;

		void	init(void) override;

		void	update(const FrameContext &ctx) override;
		void	renderUI(const Renderer &renderer) override;

	private:
		struct	EntityData {
			uint32_t	entityIndex{0};
			uint32_t	transformIndex{0};
		};
		static void	initLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants);
		static void	configurePipeline(PipelineConfig &config);

		void	renderArrow(const Renderer &renderer, Entity::id entityHandle, Entity::id arrowHandle);
		void	renderMove(const Renderer &renderer);

		void	renderScale(const Renderer &renderer);
		void	renderRotate(const Renderer &renderer);

		AssetManager			*_assetManager;
		PipelineMap				*_simplePipeline;
		Action					_action{Action::Move};
		std::vector<Entity::id>	_handles{};
};

}
