/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/19 17:21:49                                        */
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
# include "api/vulkan/Pipeline.hpp"

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
		void	render(WindowResources &resources, uint32_t currentFrame,
					uint32_t imageIndex) override;

	private:
		struct	PushConstantData {
			glm::mat4	modelMatrix;
			glm::mat4	normalMatrix;
		};

		struct	SystemPipeline {
			SystemPipeline(Camera &camera, VkFormat format, VkFormat depthFormat);
			~SystemPipeline(void);

			bool	init(void);
			bool	createPipeline(void);
			bool	createRenderPass(void);

			VkFormat		_format;
			VkFormat		_depthFormat;
			VkRenderPass	_renderPass {VK_NULL_HANDLE};
			Pipeline		_pipeline;
			Camera			&_camera;
		};
		using pipelineMap = std::unordered_map<VkFormat,
											std::unique_ptr<SystemPipeline>>;

		SystemPipeline		*getPipelineForFormat(VkFormat format, VkFormat depthFormat);
		VkPipelineLayout	getPipelineLayout(void);

		void				beginRenderPass(VkCommandBuffer commandBuffer,
											Window &window, uint32_t imageIndex,
											SystemPipeline *pipeline);
		void				endRenderPass(VkCommandBuffer commandBuffer);

		bool				_healthy {true};
		std::string			_reason {""};
		std::string			_vertPath {""};
		std::string			_fragPath {""};
		AssetManager		&_assetManager;
		pipelineMap			_pipelines;
		VkPipelineLayout	_pipelineLayout {VK_NULL_HANDLE};
};

}
