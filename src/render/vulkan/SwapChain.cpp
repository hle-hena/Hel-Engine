/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SwapChain.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 09:27:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/06 16:35:05                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/vulkan/SwapChain.hpp"
#include "platform/window/Window.hpp"
#include "render/vulkan/Device.hpp"
#include <limits>

namespace	hel {

SwapChain::SwapChain(Device &device)
	:	_device{device} {
}

SwapChain::~SwapChain(void) {
}

void	SwapChain::deleteSwapChain(void) {
	if (_swapChain != VK_NULL_HANDLE)
		vkDestroySwapchainKHR(_device.getLogical(), _swapChain, nullptr);
}

SwapChain::SupportDetails	SwapChain::querySwapChainSupport(VkPhysicalDevice &device,
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

bool	SwapChain::initiateSwapChain(Window &window) {
	SwapChain::SupportDetails	details = querySwapChainSupport(_device.getPhysical(), window.getSurface());

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
	if (vkCreateSwapchainKHR(_device.getLogical(), &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't create the swap chain", true);
	vkGetSwapchainImagesKHR(_device.getLogical(), _swapChain, &imageCount, nullptr);
	_images.resize(imageCount);
	vkGetSwapchainImagesKHR(_device.getLogical(), _swapChain, &imageCount, _images.data());
	_format = format.format;
	_extent = extent;
	return (false);
}

VkSurfaceFormatKHR	SwapChain::selectSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats) {
	for (const auto &format: formats) {
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
			&& format.format == VK_FORMAT_B8G8R8A8_SRGB)
			return (format);
	}
	return (formats[0]);
}

VkPresentModeKHR	SwapChain::selectSwapPresent(std::vector<VkPresentModeKHR> &presents) {
	for (const auto &present: presents) {
		if (present == VK_PRESENT_MODE_MAILBOX_KHR)
			return (present);
	}
	return (VK_PRESENT_MODE_FIFO_KHR);
}

VkExtent2D	SwapChain::selectSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {
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

}
