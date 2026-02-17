/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderSystem.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:14:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/16 18:09:47                                        */
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

# include "ecs/system/ISystem.hpp"
# include "api/vulkan/Pipeline.hpp"

namespace	hel {

class	AssetManager;
class	Window;

struct	PushConstantData {
	glm::mat4	modelMatrix;
	glm::mat4	normalMatrix;
};

class	RenderSystem : public ISystem {
	public:
		RenderSystem(Device &device, Registry &registry,
					VkDescriptorSetLayout &setLayout);
		~RenderSystem(void) override;

		void	render(WindowResources &resources, uint32_t currentFrame,
						uint32_t imageIndex) override;

	private:
		struct	SystemPipeline {
			SystemPipeline(RenderSystem &system, VkFormat format, VkFormat depthFormat);
			~SystemPipeline(void);

			bool	init(void);
			bool	createRenderPass(void);
			bool	createPipeline(void);

			VkFormat		_format;
			VkFormat		_depthFormat;
			Pipeline		_pipeline;
			VkRenderPass	_renderPass;
			RenderSystem	&_system;
		};
		using pipelineMap = std::unordered_map<VkFormat, std::unique_ptr<SystemPipeline>>;

		SystemPipeline		*getPipelineForFormat(VkFormat format, VkFormat depthFormat);
		VkPipelineLayout	*getPipelineLayout(void);

		void				beginRenderPass(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex, SystemPipeline *pipeline);
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
