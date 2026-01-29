/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderSystem.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:14:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/29 17:30:30                                        */
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

# include "api/vulkan/Pipeline.hpp"

# include "api/vulkan/Buffer.hpp"//TEMPORARY, since I don't have a mesh Asset yet.

namespace	hel {

class	Window;
class	Device;
class	Registry;
class	AssetManager;

class	RenderSystem {
	public:
		RenderSystem(Device &device, Registry &registry);
		~RenderSystem(void);

		RenderSystem(const RenderSystem &) = delete;
		RenderSystem	operator=(const RenderSystem &) = delete;

		struct	Vertex {
			glm::vec2	position{0.};
			glm::vec3	color{1.};

			static std::vector<VkVertexInputBindingDescription>		getBindingDescriptions(void);
			static std::vector<VkVertexInputAttributeDescription>	getAttributeDescriptions(void);
		};

		void	update(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex);

	private:
		struct	SystemPipeline {
			SystemPipeline(RenderSystem &system, VkFormat format);
			~SystemPipeline(void);

			bool	init(void);
			bool	createRenderPass(void);
			bool	createPipeline(void);

			VkFormat		_format;
			Pipeline		_pipeline;
			VkRenderPass	_renderPass;
			RenderSystem	&_system;
		};
		using pipelineMap = std::unordered_map<VkFormat, std::unique_ptr<SystemPipeline>>;

		SystemPipeline		*getPipelineForFormat(VkFormat format);
		VkPipelineLayout	*getPipelineLayout(void);

		void				beginRenderPass(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex, SystemPipeline *pipeline);
		void				endRenderPass(VkCommandBuffer commandBuffer);

		bool				_healthy{true};
		std::string			_reason{""};
		std::string			_vertPath{"assets/shaders/basic.vert.spv"};
		std::string			_fragPath{"assets/shaders/basic.frag.spv"};
		Device				&_device;
		Registry			&_registry;
		AssetManager		&_assetManager;
		pipelineMap			_pipelines;
		VkPipelineLayout	_pipelineLayout{VK_NULL_HANDLE};

		std::unique_ptr<Buffer>		_tempVertexBuffer{nullptr};
};

}
