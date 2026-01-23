/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: TriangleSystem.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/22 17:38:41                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <memory>
# include <unordered_map>

# include "api/vulkan/Pipeline.hpp"
# include "ecs/Registry.hpp"

namespace hel {

class	Window;

struct	TriangleSystemPipeline {
	public:
		TriangleSystemPipeline(Device& device, std::string vertShaderPath, std::string fragShaderPath, const VkFormat &format);
		~TriangleSystemPipeline(void);

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}

		bool	init(void);
		void	bind(VkCommandBuffer commandBuffer);
		

	private:
		bool	createRenderPass(void);
		bool	createPipelineLayout(void);
		bool	createGraphicsPipeline(void);

		bool				_healthy{true};
		std::string			_reason{""};
		std::string			_vertShaderPath;
		std::string			_fragShaderPath;
		Device				&_device;
		VkFormat			_format;
		Pipeline			_pipeline;
		VkPipelineLayout	_pipelineLayout{VK_NULL_HANDLE};
		VkRenderPass		_renderPass{VK_NULL_HANDLE};

	friend class TriangleSystem;
};

class	TriangleSystem {
	public:
		TriangleSystem(Device& device, Registry &registry, std::string vertShaderPath, std::string fragShaderPath);
		~TriangleSystem();

		TriangleSystem(const TriangleSystem &) = delete;
		TriangleSystem	&operator=(const TriangleSystem &) = delete;

		void	update(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex);

	private:
		using pipelineMap = std::unordered_map<VkFormat, std::unique_ptr<TriangleSystemPipeline>>;

		bool			_healthy{true};
		std::string		_reason{""};
		std::string		_vertShaderPath;
		std::string		_fragShaderPath;
		Device			&_device;
		Registry		&_registry;
		pipelineMap		_pipelines;

		TriangleSystemPipeline	*getPipelineForFormat(VkFormat format);

		void	beginRenderPass(VkCommandBuffer commandBuffer, TriangleSystemPipeline *pipeline,
									Window &window, uint32_t imageIndex);
		void	endRenderPass(VkCommandBuffer commandBuffer);
};

}
