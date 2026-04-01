/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:27 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/01 19:25:15                                        */
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
class	InputState;
class	Window;

}

namespace	hel::sys {

class	Selection : public ISystem {
	public:
		Selection(void) = default;
		~Selection(void) = default;

		void	init(void) override;

		void	updateWindow(const FrameContext &ctx) override;
		void	postProcessing(const Renderer &conf) override;

	private:
		struct	EntityData {
			uint32_t	entityIndex{0};
			uint32_t	transformIndex{0};
		};

		static void	configureTintPipeline(PipelineConfigInfo &config);
		static void	initEntityLayout(Device &, std::vector<VkDescriptorSetLayout> &setLayouts,
						std::vector<VkPushConstantRange> &pushConstants);
		static void	configureEntityPipeline(PipelineConfigInfo &config);

		void	renderEntityID(const Renderer &renderer);

		AssetManager	*_assetManager;
		PipelineMap		*_tintPipeline{nullptr};
		PipelineMap		*_entityIDPipeline{nullptr};
		InputState		*_inputState;
};

}
