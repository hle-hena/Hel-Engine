/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/15 16:09:04                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */


#include "render/vulkan/Device.hpp"
#include "render/vulkan/vulkanHelper.hpp"

namespace	hel {

Device::Device(VulkanInstance &instance)
	:	_instance{instance} {
}

Device::~Device(void) {
	if (!_healthy)
		return ;
	vkDestroyDevice(_device, nullptr);
}

bool	Device::pickPhysicalDevice(void) {
	uint32_t	physDeviceCount = 0;
	vkEnumeratePhysicalDevices(_instance.getVkInstance(), &physDeviceCount, nullptr);
	if (physDeviceCount == 0) {
		_healthy = false;
		_reason = "Couldn't find a GPU to pair with vulkan";
		return (true);
	}
	std::vector<VkPhysicalDevice>	physDevices(physDeviceCount);
	vkEnumeratePhysicalDevices(_instance.getVkInstance(), &physDeviceCount, physDevices.data());

	for (const auto &device: physDevices) {
		if (isDeviceSuitable(device)) {
			_physicalDevice = device;
			break ;
		}
	}
	if (_physicalDevice == VK_NULL_HANDLE) {
		_healthy = false;
		_reason = "Couldn't find a suitable physical device.";
		return (true);
	}
	return (createLogicalDevice());
}

bool	Device::isDeviceSuitable(VkPhysicalDevice device) {
	QueuesFamilyIndices	indices = findQueueFamilies(device);

	return (indices.isComplete());
}

QueuesFamilyIndices	Device::findQueueFamilies(VkPhysicalDevice device) {
	QueuesFamilyIndices	indices;
	auto	queueFamilies = enumerate<VkQueueFamilyProperties>(
		ENUMERATE_WRAP(vkGetPhysicalDeviceQueueFamilyProperties, device)
	);
	int	i = 0;
	for (const auto &queueFamily: queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;
		if (indices.isComplete())
			break ;
		i++;
	}
	return (indices);
}

bool	Device::createLogicalDevice(void) {
	QueuesFamilyIndices	indices = findQueueFamilies(_physicalDevice);
	float	priorities[] = {1.0f};
	VkDeviceQueueCreateInfo	queueCreateInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr, 0, indices.graphicsFamily.value(), 1, priorities};
	VkPhysicalDeviceFeatures	features{};
	VkDeviceCreateInfo	createInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0,
		1, &queueCreateInfo, 0, nullptr, 0, nullptr, &features};
	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) {
		_healthy = false;
		_reason = "Couldn't create a logical device";
		return (true);
	}
	vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicQueue);
	return (false);
}

}
