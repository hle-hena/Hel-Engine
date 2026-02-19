/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Render.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/19 15:48:18                                        */
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
# include "api/vulkan/Pipeline.hpp"

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

		void	render(WindowResources &resources, uint32_t currentFrame,
						uint32_t imageIndex) override;

	private:
		struct	PushConstantData {
			glm::mat4	modelMatrix;
			glm::mat4	normalMatrix;
		};

		struct	SystemPipeline {
			SystemPipeline(Render &system, VkFormat format, VkFormat depthFormat);
			~SystemPipeline(void);

			bool	init(void);
			bool	createRenderPass(void);
			bool	createPipeline(void);

			VkFormat		_depthFormat;
			VkFormat		_format;
			Pipeline		_pipeline;
			VkRenderPass	_renderPass {VK_NULL_HANDLE};
			Render			&_system;
		};
		using pipelineMap = std::unordered_map<VkFormat,
											std::unique_ptr<SystemPipeline>>;

		SystemPipeline		*getPipelineForFormat(VkFormat format, VkFormat depthFormat);
		VkPipelineLayout	getPipelineLayout(void);

		void				beginRenderPass(VkCommandBuffer commandBuffer,
											Window &window, uint32_t imageIndex,
											SystemPipeline *pipeline);
		void				endRenderPass(VkCommandBuffer commandBuffer);

		bool				_healthy{true};
		std::string			_reason{""};
		std::string			_vertPath{"assets/shaders/basic.vert.spv"};
		std::string			_fragPath{"assets/shaders/basic.frag.spv"};
		AssetManager		&_assetManager;
		pipelineMap			_pipelines;
		VkPipelineLayout	_pipelineLayout{VK_NULL_HANDLE};
};

}
