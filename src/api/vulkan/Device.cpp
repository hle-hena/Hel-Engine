/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/23 15:42:44                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Device.hpp"
#include "api/vulkan/Swapchain.hpp"
#include "api/vulkan/vulkanHelper.hpp"
#include "api/vulkan/VulkanInstance.hpp"
#include "platform/window/Window.hpp"
#include "utils/healthHelper.hpp"
#include <set>

namespace	hel {

Device::Device(VulkanInstance &instance)
	:	_instance{instance} {
}

Device::~Device(void) {
	if (_transientCommandPool != VK_NULL_HANDLE)
		vkDestroyCommandPool(_device, _transientCommandPool, nullptr);
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
	vkGetPhysicalDeviceProperties2(_physicalDevice, &_physicalProperties);
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

bool	Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t &outTypeIndex) {
	VkPhysicalDeviceMemoryProperties	deviceProperties;
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &deviceProperties);

	for (uint32_t i = 0; i < deviceProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (deviceProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			outTypeIndex = i;
			return (false);
		}
	}
	return (true);
}

bool	Device::createLogicalDevice(void) {
	std::vector<VkDeviceQueueCreateInfo>	queueCreateInfos;
	std::set<uint32_t>						uniqueQueuesFamily = {
		_indices.graphicsFamily.value(), _indices.presentFamily.value()
	};
	float	priority = 1.0f;
	for (auto queueFamily: uniqueQueuesFamily) {
		queueCreateInfos.push_back({VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			nullptr, 0, queueFamily, 1, &priority});
	}
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(_physicalDevice, &supportedFeatures);

	VkPhysicalDeviceSynchronization2Features sync2Features{};
	sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	sync2Features.synchronization2 = VK_TRUE;

	VkPhysicalDeviceDynamicRenderingFeatures	dynamicFeature{};
	dynamicFeature.pNext = &sync2Features;
	dynamicFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	dynamicFeature.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceFeatures2 features2{};
	features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features2.pNext = &dynamicFeature;

	features2.features.geometryShader = VK_TRUE;
	if (supportedFeatures.samplerAnisotropy)
		features2.features.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = &features2;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = _deviceExtensions.data();
	createInfo.pEnabledFeatures = nullptr;
	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't create a logical device", true);
	vkGetDeviceQueue(_device, _indices.graphicsFamily.value(), 0, &_graphicQueue);
	vkGetDeviceQueue(_device, _indices.presentFamily.value(), 0, &_presentQueue);
	std::cout << "Created the logical device" << std::endl;
	return (createCommandPool());
}

bool	Device::createCommandPool(void) {
	VkCommandPoolCreateInfo	commandPoolInfo{};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	commandPoolInfo.queueFamilyIndex = _indices.graphicsFamily.value();

	if (vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_transientCommandPool))
		RETURN_SET_UNHEALTHY("Couldn't create the command pool.", true);
	return (false);
}

VkCommandBuffer	Device::beginSingleTimeCommand(void) {
	VkCommandBufferAllocateInfo	allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = _transientCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer	commandBuffer;
	vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return (commandBuffer);
}

void	Device::endSingleTimeCommand(VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(_graphicQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(_graphicQueue);

	vkFreeCommandBuffers(_device, _transientCommandPool, 1, &commandBuffer);
}

bool	Device::supportSurface(Window &window) {
	VkBool32	presentSupport;
	vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, _indices.presentFamily.value(), window.getSurface(), &presentSupport);

	return (presentSupport);
}

}
