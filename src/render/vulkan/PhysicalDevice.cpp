/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PhysicalDevice.cpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/15 11:18:38                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/vulkan/PhysicalDevice.hpp"

namespace	hel {

PhysicalDevice::PhysicalDevice(VulkanInstance &instance)
	:	_instance{instance} {
}

bool	PhysicalDevice::pickPhysicalDevice(void) {
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
			_device = device;
			break ;
		}
	}
	if (_device == VK_NULL_HANDLE) {
		_healthy = false;
		_reason = "Couldn't find a suitable physical device.";
		return (true);
	}
	return (false);
}

bool	PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device) {
	return (true);
}

}
