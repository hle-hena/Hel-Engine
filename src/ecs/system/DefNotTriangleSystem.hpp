/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: DefNotTriangleSystem.hpp                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 16:58:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/26 16:59:32                                        */
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
class	AssetManager;

struct	DefNotTriangleSystemPipeline {
	public:
		DefNotTriangleSystemPipeline(Device& device, AssetManager &assetManager, std::string vertShaderPath, std::string fragShaderPath, const VkFormat &format);
		~DefNotTriangleSystemPipeline(void);

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

	friend class DefNotTriangleSystem;
};

class	DefNotTriangleSystem {
	public:
		DefNotTriangleSystem(Device& device, Registry &registry, std::string vertShaderPath, std::string fragShaderPath);
		~DefNotTriangleSystem();

		DefNotTriangleSystem(const DefNotTriangleSystem &) = delete;
		DefNotTriangleSystem	&operator=(const DefNotTriangleSystem &) = delete;

		void	update(VkCommandBuffer commandBuffer, Window &window, uint32_t imageIndex);

	private:
		using pipelineMap = std::unordered_map<VkFormat, std::unique_ptr<DefNotTriangleSystemPipeline>>;

		bool			_healthy{true};
		std::string		_reason{""};
		std::string		_vertShaderPath;
		std::string		_fragShaderPath;
		Device			&_device;
		Registry		&_registry;
		pipelineMap		_pipelines;

		DefNotTriangleSystemPipeline	*getPipelineForFormat(VkFormat format);

		void	beginRenderPass(VkCommandBuffer commandBuffer, DefNotTriangleSystemPipeline *pipeline,
									Window &window, uint32_t imageIndex);
		void	endRenderPass(VkCommandBuffer commandBuffer);
};

}
