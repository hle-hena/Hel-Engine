/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Sprite.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/16 18:25:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/16 19:03:13                                        */
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

#include "api/vulkan/Descriptors.hpp"
#include "core/Frame.hpp"
# include "ecs/systems/ISystem.hpp"
# include "api/vulkan/PipelineMap.hpp"

namespace	hel {

class	AssetManager;

}

namespace	hel::sys {

class	Sprite : public ISystem {
	public:
		Sprite(void) = default;
		~Sprite(void) = default;

		void	init(void) override;

		void	update(const FrameContext &ctx) override;
		void	render(const Renderer &renderer) override;

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

		struct	Context {
			std::vector<std::unique_ptr<DescriptorSet>>	sets;
			uint32_t	frameIndex;
		};
		std::unordered_map<RenderRequest, Context, RenderRequest::Hasher>	_frameContexts;
};

}
