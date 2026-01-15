/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 16:35:00 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/13 19:24:41                                        */
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

Renderer::Renderer(Window &window, Device &device)
	: _window{window}, _device{device}, _currentFrameIndex{0}, _isFrameStarted{false} {
	recreateSwapChain();
	createCommandBuffers();
}

Renderer::~Renderer() {
	freeCommandBuffers();
}

VkCommandBuffer Renderer::beginFrame() {
	assert(!_isFrameStarted && "Can't call beginFrame while already in progress");

	// Logic to acquire next image from _window.getSwapChain()
	// VkResult result = _window.getSwapChain()->acquireNextImage(&_currentImageIndex);
	
	// Handle resize (VK_ERROR_OUT_OF_DATE_KHR) here by calling recreateSwapChain()

	_isFrameStarted = true;
	
	// Return the command buffer for the current frame
	// return _commandBuffers[_currentFrameIndex];
	return VK_NULL_HANDLE; // Placeholder
}

void Renderer::endFrame() {
	assert(_isFrameStarted && "Can't call endFrame while frame is not in progress");

	// Logic to submit command buffer and present swapchain
	// _window.getSwapChain()->submitCommandBuffers(_commandBuffers[_currentFrameIndex], &_currentImageIndex);

	_isFrameStarted = false;
	// _currentFrameIndex = (_currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
	assert(_isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
	assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

	// VkRenderPassBeginInfo renderPassInfo{};
	// renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	// renderPassInfo.renderPass = _window.getSwapChain()->getRenderPass();
	// renderPassInfo.framebuffer = _window.getSwapChain()->getFrameBuffer(_currentImageIndex);
	
	// vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
	assert(_isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
	assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

	vkCmdEndRenderPass(commandBuffer);
}

void Renderer::createCommandBuffers() {
	// Allocate command buffers from device pool
}

void Renderer::freeCommandBuffers() {
	// Free command buffers
}

void Renderer::recreateSwapChain() {
	// Handle swapchain recreation logic, usually delegating to _window.recreateSwapChain()
	// And checking if window extent is 0,0 (minimized)
}

VkCommandBuffer Renderer::getCurrentCommandBuffer() const {
	assert(_isFrameStarted && "Cannot get command buffer when frame not in progress");
	// return _commandBuffers[_currentFrameIndex];
	return VK_NULL_HANDLE; // Placeholder
}

int Renderer::getFrameIndex() const {
	assert(_isFrameStarted && "Cannot get frame index when frame not in progress");
	return _currentFrameIndex;
}

}