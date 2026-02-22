/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/22 17:10:30                                        */
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
		Camera(Device &device, Registry &registry,
			VkDescriptorSetLayout &setLayout);
		~Camera(void) override;

		void	update(float deltaTime) override;
		void	render(VkRenderPass renderPass, WindowResources &resources,
					uint32_t currentFrame) override;

	private:
		struct	PushConstantData {
			glm::mat4	modelMatrix;
			glm::mat4	invViewProjection;
		};

		static void	initFrustumLayout(std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants);
		static void	configureFrustumPipeline(PipelineConfigInfo &config);

		bool							_healthy {true};
		std::string						_reason {""};
		AssetManager					&_assetManager;
		std::unique_ptr<PipelineMap>	_frustumPipelines;
};

}
