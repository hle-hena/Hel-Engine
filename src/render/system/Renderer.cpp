/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 16:35:00 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 15:18:58                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/system/Renderer.hpp"

#include <stdexcept>
#include <array>

namespace hel {

Renderer::Renderer(Device &device)
	:	_device{device},
		_meshSystem{device,
					"assets/shaders/triangle.vert.spv",
					"assets/shaders/triangle.frag.spv"} {
}

Renderer::~Renderer(void) {
}

void	Renderer::render(Window &window) {
	VkCommandBuffer	commandBuffer = VK_NULL_HANDLE;
	_meshSystem.render(commandBuffer, window);
}

}