/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: MeshSystem.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/13 19:30:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 12:14:49                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "render/vulkan/Pipeline.hpp"

namespace hel {

class	MeshSystem {
	public:
		MeshSystem(Device& device, Window &window);
		~MeshSystem();

		MeshSystem(const MeshSystem &) = delete;
		MeshSystem	&operator=(const MeshSystem &) = delete;

		void	render(VkCommandBuffer commandBuffer);
		bool	initMeshSystem();

	private:
		bool	createRenderPass(void);
		bool	createPipelineLayout(void);
		bool	createGraphicsPipeline(void);

		bool				_healthy{true};
		std::string			_reason{""};
		Device				&_device;
		Window				&_window;
		Pipeline			_pipeline;
		VkPipelineLayout	_pipelineLayout{VK_NULL_HANDLE};
		VkRenderPass		_renderPass{VK_NULL_HANDLE};
};

}
