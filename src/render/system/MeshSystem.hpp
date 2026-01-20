/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: MeshSystem.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/13 19:30:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 19:30:21                                        */
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

# include "render/vulkan/Pipeline.hpp"

namespace hel {

class	Window;

struct	MeshSystemPipeline {
	public:
		MeshSystemPipeline(Device& device, std::string vertShaderPath, std::string fragShaderPath, const VkFormat &format);
		~MeshSystemPipeline(void);

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

	friend class MeshSystem;
};

class	MeshSystem {
	public:
		MeshSystem(Device& device, std::string vertShaderPath, std::string fragShaderPath);
		~MeshSystem();

		MeshSystem(const MeshSystem &) = delete;
		MeshSystem	&operator=(const MeshSystem &) = delete;

		void	render(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex);

	private:
		using pipelineMap = std::unordered_map<VkFormat, std::unique_ptr<MeshSystemPipeline>>;

		bool			_healthy{true};
		std::string		_reason{""};
		std::string		_vertShaderPath;
		std::string		_fragShaderPath;
		Device			&_device;
		pipelineMap		_pipelines;

		MeshSystemPipeline	*getPipelineForFormat(VkFormat format);

		void	beginRenderPass(VkCommandBuffer commandBuffer, MeshSystemPipeline *pipeline,
									Window &window, uint32_t imageIndex);
		void	endRenderPass(VkCommandBuffer commandBuffer);
};

}
