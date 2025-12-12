/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/11 10:10:31 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/12 18:47:23                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/vulkan/Device.hpp"
#include "render/vulkan/vulkanHelper.hpp"

#include <iostream>
#include <cstring>

namespace	hel {

Device::Device(void) {
	createInstance();
	if (!_healthy)
		return ;
	setupDebugMessenger();
}

Device::~Device(void) {
	if (!_healthy)
		return ;//TODO -> diagnostic what went wrong ? Maybe there are some things to free I think ?
	if (enableValidationLayers)
		CALL_VKINSTANCE_FUNC_VOID(_instance, vkDestroyDebugUtilsMessengerEXT,
								_debugMessenger, nullptr);
	vkDestroyInstance(_instance, nullptr);
}

void	Device::createInstance() {
	if (Device::enableValidationLayers && !checkValidationLayerSupport()) {
		_healthy = false;
		_reason = "Missing support for a validation layer";//TODO -> make this message clearer.
		return ;
	}

	VkApplicationInfo	appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr, "Hel", VK_MAKE_VERSION(0, 0, 0),
		"Hel Engine", VK_MAKE_VERSION(0, 0, 0), VK_API_VERSION_1_2
	};
	std::vector<const char *>	extensions = getExtensions();
	if (Device::enableValidationLayers && !checkExtensionSupport(extensions)) {
		_healthy = false;
		_reason = "Missing support for a extension";//TODO -> make this message clearer.
		return ;
	}

	VkInstanceCreateInfo	createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr, 0, &appInfo, 0, nullptr,
		static_cast<uint32_t>(extensions.size()), extensions.data()
	};
	VkDebugUtilsMessengerCreateInfoEXT	debugInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
		createInfo.ppEnabledLayerNames = _validationLayers.data();

		populateMessengerCreateInfo(debugInfo);
		createInfo.pNext = &debugInfo;
	}

	if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
		_healthy = false;
		_reason = "Failded to create an instance of vulkan";
		return ;
	}
}

std::vector<const char *>	Device::getExtensions(void) {
	uint32_t	glfwExtensionsCount = 0;
	const char	**glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

	std::vector<const char *>	extensions(glfwExtensions,
										glfwExtensions + glfwExtensionsCount);
	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return (extensions);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL	debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void	Device::populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
}

void	Device::setupDebugMessenger(void) {
	VkDebugUtilsMessengerCreateInfoEXT	createInfo{};
	populateMessengerCreateInfo(createInfo);
	VkResult	res = VK_SUCCESS;
	CALL_VKINSTANCE_FUNC_VKRESULT(res, _instance, vkCreateDebugUtilsMessengerEXT,
									&createInfo, nullptr, &_debugMessenger);
	if (res != VK_SUCCESS) {
		_healthy = false;
		_reason = "Failed to create the messenger";
	}
}

bool	Device::checkValidationLayerSupport(void) {
	uint32_t	layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties>	availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char *layerName: _validationLayers) {
		bool	layerFound = false;
		for (const VkLayerProperties &availableLayer: availableLayers) {
			if (std::strcmp(layerName, availableLayer.layerName) == 0) {
				layerFound = true;
				break ;
			}
		}
		if (!layerFound)
			return (false);
	}
	std::cout << "All validation layers have been found" << std::endl;
	return (true);
}

bool	Device::checkExtensionSupport(std::vector<const char *> &extensions) {
	uint32_t	extensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
	std::vector<VkExtensionProperties>	availableExtensions(extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount,
										availableExtensions.data());

	for (const char *extensionName: extensions) {
		bool	extensionFound = false;
		for (const VkExtensionProperties &availableExtensions: availableExtensions) {
			if (std::strcmp(extensionName, availableExtensions.extensionName) == 0) {
				extensionFound = true;
				break ;
			}
		}
		if (!extensionFound)
			return (false);
	}
	std::cout << "All extensions have been found" << std::endl;
	std::cout << "Available extensions:" << std::endl;
	for (const auto &extension: availableExtensions) {
		std::cout << "\t" << extension.extensionName << ":"
			<< extension.specVersion << std::endl;
	}
	return (true);
}

}