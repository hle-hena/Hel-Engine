/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Render.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/03 16:04:18                                        */
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

class	Render : public ISystem {
	public:
		Render(void) = default;
		~Render(void) = default;

		void	init(void) override;

		void	render(const Renderer &conf) override;

	private:
		struct	EntityData {
			uint32_t	entityIndex{0};
			uint32_t	transformIndex{0};
		};

		static void	initLayout(Device &, std::vector<VkDescriptorSetLayout> &setLayouts,
						std::vector<VkPushConstantRange> &pushConstants);
		static void	configureNormalPipeline(PipelineConfig &config);
		static void	configureSelectedPipeline(PipelineConfig &config);

		AssetManager	*_assetManager;
		PipelineMap		*_normalPipeline{nullptr};
		PipelineMap		*_selectedObjectPipeline{nullptr};
};

}
