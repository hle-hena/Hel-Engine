/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Sprite.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/16 18:25:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/06 10:57:28                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "HelSystem.hpp"

namespace	hel::sys {

class	Sprite : public ISystem {
	public:
		Sprite(void) = default;
		~Sprite(void) = default;

		void	init(void) override;

		void	render(const Renderer &renderer);

	private:
		struct	EntityData {
			uint32_t	entityIndex{0};
			uint32_t	transformIndex{0};
			uint32_t	cameraIndex{0};
		};

		static void	initLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants);
		static void	configurePipeline(PipelineConfig &config);

		AssetManager	*_assetManager;
		PipelineMap		*_pipeline;
};

}
