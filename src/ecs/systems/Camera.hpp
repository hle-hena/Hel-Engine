/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/12 14:18:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <cstdint>
# include <glm/glm.hpp>

# include "ecs/systems/ISystem.hpp"
# include "api/vulkan/PipelineMap.hpp"

namespace	hel {

class	AssetManager;

}

namespace	hel::sys {

class	Camera : public ISystem {
	public:
		Camera(void) = default;
		~Camera(void) = default;

		void	init(void) override;

		void	update(const FrameContext &ctx);
		void	renderInteraction(const Renderer &renderer) override;

		RENDER_TYPES("RenderScene")

	private:
		struct	FrustumPush {
			glm::mat4	modelMatrix;
			glm::mat4	invViewProjection;
		};
		struct	EntityData {
			uint32_t	entityIndex{0};
			uint32_t	transformIndex{0};
			float		size{0};
		};

		static void	initFrustumLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants);
		static void	configureFrustumPipeline(PipelineConfig &config);
		static void	initSpriteLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants);
		static void	configureSpritePipeline(PipelineConfig &config);

		AssetManager	*_assetManager;
		PipelineMap		*_frustumPipeline;
		PipelineMap		*_spritePipeline;
};

}
