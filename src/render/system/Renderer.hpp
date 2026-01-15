/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 16:35:00 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 12:16:31                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <cassert>

namespace hel {

class	Window;
class	Device;

class	Renderer {
	public:
		Renderer(Window &window, Device &device);
		~Renderer();

		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;

		VkCommandBuffer	beginFrame(void);
		void			endFrame(void);
		void			beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void			endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		bool			isFrameInProgress(void) const { return _isFrameStarted; }
		VkCommandBuffer	getCurrentCommandBuffer(void) const;
		int				getFrameIndex(void) const;

	private:
		Window			&_window;
		Device			&_device;
		// Add command buffers, semaphores, and fences here
		// std::vector<VkCommandBuffer> _commandBuffers;
		uint32_t		_currentImageIndex;
		int				_currentFrameIndex;
		bool			_isFrameStarted;

		void	createCommandBuffers(void);
		void	freeCommandBuffers(void);
		void	recreateSwapChain(void);
};

}