/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/06 17:50:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/vulkan/Device.hpp"
#include "render/vulkan/Swapchain.hpp"
#include "render/vulkan/vulkanHelper.hpp"
#include "utils/healthHelper.hpp"
#include <set>

namespace	hel {

Device::Device(VulkanInstance &instance)
	:	_instance{instance} {
}

Device::~Device(void) {
	if (_device != VK_NULL_HANDLE)
		vkDestroyDevice(_device, nullptr);
}

bool	Device::pickPhysicalDevice(Window &bootstrapWindow) {
	auto	physDevices = enumerate<VkPhysicalDevice>(
		ENUMERATE_WRAP(vkEnumeratePhysicalDevices, _instance.getVkInstance())
	);
	if (physDevices.size() == 0)
		RETURN_SET_UNHEALTHY("Couldn't find a GPU to pair with vulkan", true);

	for (const auto &device: physDevices) {
		if (isDeviceSuitable(device, bootstrapWindow)) {
			_physicalDevice = device;
			break ;
		}
	}
	if (_physicalDevice == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Couldn't find a suitable physical device.", true);
	return (createLogicalDevice());
}

bool	Device::isDeviceSuitable(VkPhysicalDevice device, Window &bootstrapWindow) {
	QueuesFamilyIndices	indices = findQueueFamilies(device, bootstrapWindow);
	bool				extensionsSupported = checkDeviceExtensionSupport(device);
	bool				swapChainAdequate = false;

	if (indices.isComplete() && extensionsSupported) {
		Swapchain::SupportDetails	details = Swapchain::querySwapChainSupport(device, bootstrapWindow.getSurface());
		swapChainAdequate = !details.formats.empty() && !details.presents.empty();
		if (!swapChainAdequate)
			return (false);
		_indices = indices;
		return (true);
	}
	return (false);
}

bool	Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	auto				availableExtensions = enumerate<VkExtensionProperties>(
		ENUMERATE_WRAP(vkEnumerateDeviceExtensionProperties, device, nullptr)
	);
	std::set<std::string>	reqExtensions(_deviceExtensions.begin(), _deviceExtensions.end());
	for (const auto &extension: availableExtensions) {
		reqExtensions.erase(extension.extensionName);
	}
	return (reqExtensions.empty());
}

QueuesFamilyIndices	Device::findQueueFamilies(VkPhysicalDevice device, Window &bootstrapWindow) {
	QueuesFamilyIndices	indices;
	auto	queueFamilies = enumerate<VkQueueFamilyProperties>(
		ENUMERATE_WRAP(vkGetPhysicalDeviceQueueFamilyProperties, device)
	);
	int	i = 0;
	for (const auto &queueFamily: queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;
		VkBool32	presentSupport;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, bootstrapWindow.getSurface(), &presentSupport);
		if (presentSupport)
			indices.presentFamily = i;
		if (indices.isComplete())
			break ;
		i++;
	}
	return (indices);
}

bool	Device::createLogicalDevice() {
	std::vector<VkDeviceQueueCreateInfo>	queueCreateInfos;
	std::set<uint32_t>						uniqueQueuesFamily = {
		_indices.graphicsFamily.value(), _indices.presentFamily.value()
	};
	float	priority = 1.0f;
	for (auto queueFamily: uniqueQueuesFamily) {
		queueCreateInfos.push_back({VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			nullptr, 0, queueFamily, 1, &priority});
	}
	VkPhysicalDeviceFeatures	features{};
	VkDeviceCreateInfo	createInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0,
		static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(),
		0, nullptr, static_cast<uint32_t>(_deviceExtensions.size()), _deviceExtensions.data(), &features};
	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't create a logical device", true);
	vkGetDeviceQueue(_device, _indices.graphicsFamily.value(), 0, &_graphicQueue);
	vkGetDeviceQueue(_device, _indices.presentFamily.value(), 0, &_presentQueue);
	std::cout << "Created the logical device" << std::endl;
	return (false);
}

bool	Device::supportSurface(Window &window) {
	VkBool32	presentSupport;
	vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, _indices.presentFamily.value(), window.getSurface(), &presentSupport);

	return (presentSupport);
}

}
