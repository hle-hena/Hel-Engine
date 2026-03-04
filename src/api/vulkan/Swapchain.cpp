/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Swapchain.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 09:27:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/04 15:51:46                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Swapchain.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/Device.hpp"
#include "utils/healthHelper.hpp"
#include "api/vulkan/MemoryHelper.hpp"

#include <limits>
#include <iostream>

namespace	hel {

Swapchain::Swapchain(Device &device)
	:	_device{device} {
}

Swapchain::~Swapchain(void) {
}

void	Swapchain::deleteSwapChain(void) {
	vkDeviceWaitIdle(_device.getLogical());
	for (auto it : _frameBufferCache) {
		for (auto frameBuffer : it.second) {
			vkDestroyFramebuffer(_device.getLogical(), frameBuffer, nullptr);
		}
	}
	_frameBufferCache.clear();
	for (auto imageView : _imagesView) {
		vkDestroyImageView(_device.getLogical(), imageView, nullptr);
	}
	_imagesView.clear();
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (_renderFinished[i] != VK_NULL_HANDLE)
			vkDestroySemaphore(_device.getLogical(), _renderFinished[i], nullptr);
		if (_imageAvailable[i] != VK_NULL_HANDLE)
			vkDestroySemaphore(_device.getLogical(), _imageAvailable[i], nullptr);
		if (_inFlightFences[i] != VK_NULL_HANDLE)
			vkDestroyFence(_device.getLogical(), _inFlightFences[i], nullptr);
	}
	_renderFinished.fill(VK_NULL_HANDLE);
	_imageAvailable.fill(VK_NULL_HANDLE);
	_inFlightFences.fill(VK_NULL_HANDLE);
	if (_swapchain != VK_NULL_HANDLE)
		vkDestroySwapchainKHR(_device.getLogical(), _swapchain, nullptr);
}

Swapchain::SupportDetails	Swapchain::querySwapChainSupport(VkPhysicalDevice &device,
															VkSurfaceKHR surface) {
	SupportDetails	details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	uint32_t	count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
	if (count != 0) {
		details.formats.resize(count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, details.formats.data());
	}
	count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
	if (count != 0) {
		details.presents.resize(count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, details.presents.data());
	}
	return (details);
}

bool	Swapchain::initiateSwapChain(Window &window) {
	Swapchain::SupportDetails	details = querySwapChainSupport(_device.getPhysical(), window.getSurface());

	VkSurfaceFormatKHR	format = selectSwapSurfaceFormat(details.formats);
	VkPresentModeKHR	present = selectSwapPresent(details.presents);
	VkExtent2D			extent = selectSwapExtent(details.capabilities, window.getWindow());
	uint32_t			imageCount = details.capabilities.maxImageCount == 0 ?
		(details.capabilities.minImageCount + 1) :
		(std::min(details.capabilities.minImageCount + 1, details.capabilities.maxImageCount));
	VkSwapchainCreateInfoKHR	createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = window.getSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	QueuesFamilyIndices	indices = _device.getQueueFamily();
	uint32_t			pIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = pIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = present;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	if (vkCreateSwapchainKHR(_device.getLogical(), &createInfo, nullptr, &_swapchain) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't create the swap chain", true);
	vkGetSwapchainImagesKHR(_device.getLogical(), _swapchain, &imageCount, nullptr);
	_images.resize(imageCount);
	vkGetSwapchainImagesKHR(_device.getLogical(), _swapchain, &imageCount, _images.data());
	_format = format.format;
	_extent = extent;
	return (createSwapchainImageView() || createDepthResources() || createSyncObjects());
}

bool	Swapchain::recreateSwapChain(Window &window) {
	vkDeviceWaitIdle(_device.getLogical());

	deleteSwapChain();

	return (initiateSwapChain(window));
}

VkSurfaceFormatKHR	Swapchain::selectSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats) {
	for (const auto &format: formats) {
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
			&& format.format == VK_FORMAT_B8G8R8A8_SRGB)
			return (format);
	}
	return (formats[0]);
}

VkPresentModeKHR	Swapchain::selectSwapPresent(std::vector<VkPresentModeKHR> &presents) {
	for (const auto &present: presents) {
		if (present == VK_PRESENT_MODE_MAILBOX_KHR)
			return (present);
	}
	return (VK_PRESENT_MODE_FIFO_KHR);
}

VkExtent2D	Swapchain::selectSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return (capabilities.currentExtent);
	int	width, height;
	glfwGetWindowSize(window, &width, &height);

	VkExtent2D extent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};
	extent.width = std::clamp(extent.width, capabilities.minImageExtent.width,
							capabilities.maxImageExtent.width);
	extent.height = std::clamp(extent.height, capabilities.minImageExtent.height,
							capabilities.maxImageExtent.height);
	return (extent);
}

bool	Swapchain::createImageView(VkImage &image, VkImageView &imageView,
								VkFormat format, VkImageAspectFlags aspectFlag) {
	VkImageViewCreateInfo	createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = aspectFlag;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	if (vkCreateImageView(_device.getLogical(), &createInfo, nullptr, &imageView))
		return (true);
	return (false);
}

bool	Swapchain::createSwapchainImageView(void) {
	_imagesView.resize(_images.size());
	for (size_t i = 0; i < _images.size(); i++) {
		if (createImageView(_images[i], _imagesView[i], _format, VK_IMAGE_ASPECT_COLOR_BIT))
			RETURN_SET_UNHEALTHY("Couldn't create an image view", true);
	}
	return (false);
}

bool	Swapchain::createDepthResources(void) {
	_depthFormat = selectDepthFormat(
		{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);

	Image::Config	config{};
	config.format = _depthFormat;
	config.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	config.width = _extent.width;
	config.height = _extent.height;
	config.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	config.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
	_depthImage = Image::create(_device, config);
	return (!_depthImage);
}

VkFormat	Swapchain::selectDepthFormat(const std::vector<VkFormat> &candidates,
										VkImageTiling tiling,
										VkFormatFeatureFlags features) {
	for (VkFormat format: candidates) {
		VkFormatProperties	properties;
		vkGetPhysicalDeviceFormatProperties(_device.getPhysical(), format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR &&
			(properties.linearTilingFeatures & features) == features)
			return (format);
		else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
			(properties.optimalTilingFeatures & features) == features)
			return (format);
	}

	return (VK_FORMAT_UNDEFINED);
}

VkFramebuffer	Swapchain::getFrameBuffer(uint32_t imageIndex, VkRenderPass renderPass) {
	if (_frameBufferCache.find(renderPass) == _frameBufferCache.end() &&
		createFramebuffersForRenderPass(renderPass))
		return (VK_NULL_HANDLE);
	return (_frameBufferCache[renderPass][imageIndex]);
}

bool	Swapchain::createFramebuffersForRenderPass(VkRenderPass renderPass) {
	_frameBufferCache[renderPass] = std::vector<VkFramebuffer>(_imagesView.size());
	for (size_t i = 0; i < _imagesView.size(); i++) {
		std::array<VkImageView, 2>	attachments = {
			_imagesView[i],
			_depthImage->getView()
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = _extent.width;
		framebufferInfo.height = _extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(_device.getLogical(), &framebufferInfo,
								nullptr, &_frameBufferCache[renderPass][i])
								!= VK_SUCCESS)
			return (true);
	}
	return (false);
}

bool	Swapchain::createSyncObjects(void) {
	VkSemaphoreCreateInfo	semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo	fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(_device.getLogical(), &semaphoreInfo, nullptr, &_imageAvailable[i]) != VK_SUCCESS ||
			vkCreateSemaphore(_device.getLogical(), &semaphoreInfo, nullptr, &_renderFinished[i]) != VK_SUCCESS ||
			vkCreateFence(_device.getLogical(), &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
			RETURN_SET_UNHEALTHY("Failed to create synchronization object for a swapchain", true);
	}

	return (false);
}

bool	Swapchain::acquireNextImage(Window &window, uint32_t currentFrame, uint32_t *imageIndex) {
	vkWaitForFences(_device.getLogical(), 1, &_inFlightFences[currentFrame],
					VK_TRUE, UINT64_MAX);

	VkResult	result = vkAcquireNextImageKHR(_device.getLogical(), _swapchain,
												UINT64_MAX, _imageAvailable[currentFrame],
												VK_NULL_HANDLE, imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || _frameBufferResized) {
		recreateSwapChain(window);
		_frameBufferResized = false;
		return (true);
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		std::cerr << "Failed to acquire a swapchain image" << std::endl;
		return (true);
	}
	vkResetFences(_device.getLogical(), 1, &_inFlightFences[currentFrame]);
	return (false);
}

bool	Swapchain::submitCommandBuffer(VkCommandBuffer *commandBuffer,
									uint32_t imageIndex, uint32_t currentFrame) {
	VkSubmitInfo	submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore	waitSemaphores[] = {_imageAvailable[currentFrame]};
	VkPipelineStageFlags	waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer;

	VkSemaphore	signalSemaphores[] = {_renderFinished[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(_device.getGraphicsQueue(), 1, &submitInfo,
				_inFlightFences[currentFrame]) != VK_SUCCESS) {
		std::cerr << "Failed to submit the command buffer" << std::endl;
		return (true);
	}
	return (false);
}

bool	Swapchain::present(Window &window, uint32_t imageIndex, uint32_t currentFrame) {
	VkPresentInfoKHR	presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &_renderFinished[currentFrame];

	VkSwapchainKHR	swapChains[] = {_swapchain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	VkResult	result = vkQueuePresentKHR(_device.getPresentQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _frameBufferResized) {
		recreateSwapChain(window);
		_frameBufferResized = false;
		return (true);
	}
	else if (result != VK_SUCCESS) {
		std::cerr << "Failed to present swap chain image" << std::endl;
		return (true);
	}
	return (false);
}

}
