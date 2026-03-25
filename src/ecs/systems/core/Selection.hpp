/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:27 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/25 10:36:45                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <glm/glm.hpp>

# include "ecs/systems/ISystem.hpp"
# include "api/vulkan/PipelineMap.hpp"

namespace	hel {

class	AssetManager;
class	Window;

}

namespace	hel::sys {

class	Selection : public ISystem {
	public:
		Selection(void) = default;
		~Selection(void) = default;

		void	init(void) override;

		void	update(const FrameContext &ctx) override;
		void	render(const FrameContext &ctx, const Renderer &conf) override;

	private:
		struct	PushConstantData {
			glm::mat4	modelMatrix;
			glm::mat4	normalMatrix;
		};

		static void	initLayout(Device &, std::vector<VkDescriptorSetLayout> &setLayouts,
						std::vector<VkPushConstantRange> &pushConstants);
		static void	configurePipeline(PipelineConfigInfo &config);

		AssetManager	*_assetManager;
		PipelineMap		*_pipeline{nullptr};
};

}
