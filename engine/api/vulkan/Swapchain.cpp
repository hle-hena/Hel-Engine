/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Swapchain.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 09:27:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 17:30:17                                        */
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
#include "api/vulkan/Image.hpp"

#include <limits>
#include <iostream>
#include <algorithm>

namespace	hel {

Swapchain::Swapchain(Device &device)
	:	_device{device} {
}

Swapchain::~Swapchain(void) {
}

void	Swapchain::deleteSwapChain(void) {
	vkDeviceWaitIdle(_device.getLogical());
	_swapImages.clear();
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (_inFlightFences[i] != VK_NULL_HANDLE)
			vkDestroyFence(_device.getLogical(), _inFlightFences[i], nullptr);
	}
	for (size_t i = 0; i < _renderFinished.size(); i++) {
		if (_renderFinished[i] != VK_NULL_HANDLE)
			vkDestroySemaphore(_device.getLogical(), _renderFinished[i], nullptr);
		if (_imageAvailable[i] != VK_NULL_HANDLE)
			vkDestroySemaphore(_device.getLogical(), _imageAvailable[i], nullptr);
	}
	_renderFinished.clear();
	_imageAvailable.clear();
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

expected<void>	Swapchain::initiateSwapChain(Window &window) {
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
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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
		return unexpected("Couldn't create the swap chain");
	vkGetSwapchainImagesKHR(_device.getLogical(), _swapchain, &imageCount, nullptr);
	std::vector<VkImage>	images(imageCount);
	vkGetSwapchainImagesKHR(_device.getLogical(), _swapchain, &imageCount, images.data());

	for (auto image: images)
		_swapImages.emplace_back(Image::wrapSwapchainImages(_device, image,
													format.format, extent));

	return (createSyncObjects());
}

expected<void>	Swapchain::recreateSwapChain(Window &window) {
	vkDeviceWaitIdle(_device.getLogical());

	deleteSwapChain();

	return (initiateSwapChain(window));
}

VkSurfaceFormatKHR	Swapchain::selectSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats) {
	for (const auto &format: formats) {
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
			&& format.format == VK_FORMAT_B8G8R8A8_UNORM)
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

expected<void>	Swapchain::createSyncObjects(void) {
	size_t	imageCount = _swapImages.size();
	_imageAvailable.resize(imageCount, VK_NULL_HANDLE);
	_renderFinished.resize(imageCount, VK_NULL_HANDLE);
	VkSemaphoreCreateInfo	semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	for (size_t i = 0; i < imageCount; i++) {
		if (vkCreateSemaphore(_device.getLogical(), &semaphoreInfo, nullptr,
				&_imageAvailable[i]) != VK_SUCCESS
			|| vkCreateSemaphore(_device.getLogical(), &semaphoreInfo, nullptr,
				&_renderFinished[i]) != VK_SUCCESS)
			return unexpected("Failed to create a semaphore for a swapchain");
	}

	VkFenceCreateInfo	fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (size_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateFence(_device.getLogical(), &fenceInfo, nullptr,
				&_inFlightFences[i]) != VK_SUCCESS)
			return unexpected("Failed to create a fence for a swapchain");
	}
	return {};
}

Image	*Swapchain::getSwapImage(uint32_t imageIndex) {
	return (_swapImages[imageIndex].get());
}

void	Swapchain::waitForFrameFence(uint32_t frameIndex) {
	vkWaitForFences(_device.getLogical(), 1, &_inFlightFences[frameIndex],
					VK_TRUE, UINT64_MAX);
	vkResetFences(_device.getLogical(), 1, &_inFlightFences[frameIndex]);
}

bool	Swapchain::acquireNextImage(Window &window, uint32_t currentFrame, uint32_t *imageIndex) {
	VkResult	result = vkAcquireNextImageKHR(_device.getLogical(), _swapchain,
												UINT64_MAX, _imageAvailable[currentFrame],
												VK_NULL_HANDLE, imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || _frameBufferResized) {
		auto	res = recreateSwapChain(window);
		if (!res)
			std::cerr << "Unhandled swapchain recreation fail.\n";
		_frameBufferResized = false;
		return (true);
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		std::cerr << "Failed to acquire a swapchain image" << std::endl;
		return (true);
	}
	return (false);
}

bool	Swapchain::submitCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame) {
	VkSubmitInfo	submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore	waitSemaphores[] = {_imageAvailable[currentFrame]};
	VkPipelineStageFlags	waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore	signalSemaphores[] = {_renderFinished[imageIndex]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(_device.getGraphicsQueue(), 1, &submitInfo,
				_inFlightFences[currentFrame]) != VK_SUCCESS) {
		std::cerr << "Failed to submit the command buffer" << std::endl;
		return (true);
	}
	return (false);
}

bool	Swapchain::present(Window &window, uint32_t imageIndex) {
	VkPresentInfoKHR	presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &_renderFinished[imageIndex];
	VkSwapchainKHR	swapChains[] = {_swapchain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	VkResult	result = vkQueuePresentKHR(_device.getPresentQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _frameBufferResized) {
		auto	res = recreateSwapChain(window);
		if (!res)
			std::cerr << "Unhandled swapchain recreation fail.\n";
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
