/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Render.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/22 16:53:40                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <unordered_map>
# include <memory>
# include <string>
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
		Render(Device &device, Registry &registry,
					VkDescriptorSetLayout &setLayout);
		~Render(void) override;

		void	render(VkRenderPass renderPass, WindowResources &resources,
					uint32_t currentFrame) override;

	private:
		struct	PushConstantData {
			glm::mat4	modelMatrix;
			glm::mat4	normalMatrix;
		};

		static void	initLayout(std::vector<VkDescriptorSetLayout> &setLayouts,
						std::vector<VkPushConstantRange> &pushConstants);
		static void	configurePipeline(PipelineConfigInfo &config);

		bool							_healthy{true};
		std::string						_reason{""};
		std::string						_vertPath{"assets/shaders/basic.vert.spv"};
		std::string						_fragPath{"assets/shaders/basic.frag.spv"};
		AssetManager					&_assetManager;
		std::unique_ptr<PipelineMap>	_pipelines{nullptr};
};

}
