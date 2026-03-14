/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 19:10:44                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <unordered_map>
# include <memory>
# include <glm/glm.hpp>

# include "ecs/systems/ISystem.hpp"
# include "api/vulkan/PipelineMap.hpp"

namespace	hel {

class	AssetManager;
class	Window;

}

namespace	hel::sys {

class	Camera : public ISystem {
	public:
		Camera(void) = default;
		~Camera(void) = default;

		void	init(void) override;

		void	update(const FrameContext &ctx) override;
		void	render(const FrameContext &ctx,
					const RenderingConfig &conf) override;

	private:
		struct	PushConstantData {
			glm::mat4	modelMatrix;
			glm::mat4	invViewProjection;
		};

		static void	initFrustumLayout(std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants);
		static void	configureFrustumPipeline(PipelineConfigInfo &config);

		AssetManager	*_assetManager;
		PipelineMap		*_frustumPipelines;
};

}
