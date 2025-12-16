/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/16 20:14:32                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/vulkan/Device.hpp"
#include "render/vulkan/vulkanHelper.hpp"
#include "utils/healthHelper.hpp"

namespace	hel {

Device::Device(VulkanInstance &instance)
	:	_instance{instance} {
}

Device::~Device(void) {
	if (_device != VK_NULL_HANDLE)
		vkDestroyDevice(_device, nullptr);
}

bool	Device::pickPhysicalDevice(void) {
	auto	physDevices = enumerate<VkPhysicalDevice>(
		ENUMERATE_WRAP(vkEnumeratePhysicalDevices, _instance.getVkInstance())
	);
	if (physDevices.size() == 0)
		RETURN_SET_UNHEALTHY("Couldn't find a GPU to pair with vulkan", true);

	for (const auto &device: physDevices) {
		if (isDeviceSuitable(device)) {
			_physicalDevice = device;
			break ;
		}
	}
	if (_physicalDevice == VK_NULL_HANDLE)
		RETURN_SET_UNHEALTHY("Couldn't find a suitable physical device.", true);
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
	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't create a logical device", true);
	vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicQueue);
	std::cout << "Created the logical device" << std::endl;
	return (false);
}

}
