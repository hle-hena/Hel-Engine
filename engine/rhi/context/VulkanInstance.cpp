/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanInstance.cpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:33:30 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 10:28:26                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/context/VulkanInstance.hpp"
#include "rhi/context/validationLayer.hpp"
#include "rhi/context/vulkanHelper.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace	hel {

VulkanInstance::~VulkanInstance(void) {
	DESTROY_DEBUG_MESSENGER();
	if (_instance != VK_NULL_HANDLE)
		vkDestroyInstance(_instance, nullptr);
}

expected<void>	VulkanInstance::createInstance() {
	std::vector<const char *>	reqExt = getExtensions();
	if (!checkAllSupport(reqExt))
		return tl::unexpected("A required extension wasn't found.");

	VkApplicationInfo	appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr, "Hel", VK_MAKE_VERSION(0, 0, 0),
		"Hel Engine", VK_MAKE_VERSION(0, 0, 0), VK_API_VERSION_1_3
	};
	VkInstanceCreateInfo	createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr, 0, &appInfo, 0, nullptr,
		static_cast<uint32_t>(reqExt.size()), reqExt.data()
	};
	VkDebugUtilsMessengerCreateInfoEXT	debugInfo{};
	ADD_VALIDATION_LAYERS();

	if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
		return tl::unexpected("Failed to create the VkInstance.");
	return SETUP_DEBUG_MESSENGER();
}

std::vector<const char *>	VulkanInstance::getExtensions(void) {
	uint32_t	glfwExtensionsCount = 0;
	const char	**glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	std::vector<const char *>	reqExt(glfwExtensions, glfwExtensions + glfwExtensionsCount);
	ADD_VALIDATION_LAYER_SUPPORT();
	return (reqExt);
}

bool	VulkanInstance::checkAllSupport(std::vector<const char *> &reqExt) {
	auto	availableExt = enumerate<VkExtensionProperties>(
		ENUMERATE_WRAP(vkEnumerateInstanceExtensionProperties, nullptr)
	);
	if (checkSupport("extension", reqExt, availableExt,
					[](const VkExtensionProperties &p){ return (p.extensionName); }))
		return (true);

	CHECK_SUPPORT_VALIDATION_LAYER();
	return (false);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL	debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT /* messageSeverity */,
		VkDebugUtilsMessageTypeFlagsEXT /* messageType */,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* /* pUserData */) {
	std::cerr << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void	VulkanInstance::populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
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

expected<void>	VulkanInstance::setupDebugMessenger(void) {
	VkDebugUtilsMessengerCreateInfoEXT	createInfo{};
	populateMessengerCreateInfo(createInfo);
	VkResult	res = VK_SUCCESS;
	CALL_VKINSTANCE_FUNC_VKRESULT(res, _instance, vkCreateDebugUtilsMessengerEXT,
									&createInfo, nullptr, &_debugMessenger);
	if (res != VK_SUCCESS)
		return unexpected("Failed to create the debug messenger.");
	return {};
}

}
