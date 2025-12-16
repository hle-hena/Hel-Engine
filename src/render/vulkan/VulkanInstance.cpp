/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanInstance.cpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:33:30 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/15 16:03:52                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/vulkan/VulkanInstance.hpp"
#include "render/vulkan/validationLayer.hpp"
#include "render/vulkan/vulkanHelper.hpp"

namespace	hel {

VulkanInstance::~VulkanInstance(void) {
	if (!_healthy)
		return ;//TODO -> diagnostic what went wrong ? Maybe there are some things to free I think ?
	DESTROY_DEBUG_MESSENGER();
	vkDestroyInstance(_instance, nullptr);
}

bool	VulkanInstance::createInstance() {
	std::vector<const char *>	reqExt = getExtensions();
	if (!checkAllSupport(reqExt))
		return (true);

	VkApplicationInfo	appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr, "Hel", VK_MAKE_VERSION(0, 0, 0),
		"Hel Engine", VK_MAKE_VERSION(0, 0, 0), VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo	createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr, 0, &appInfo, 0, nullptr,
		static_cast<uint32_t>(reqExt.size()), reqExt.data()
	};
	VkDebugUtilsMessengerCreateInfoEXT	debugInfo{};
	ADD_VALIDATION_LAYERS();

	if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
		_healthy = false;
		_reason = "Failded to create an instance of vulkan";
		return (true);
	}
	return (SETUP_VALIDATION_LAYER());
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
	if (!checkSupport("extension", reqExt, availableExt,
					[](const VkExtensionProperties &p){ return (p.extensionName); }))
		return (false);

	CHECK_SUPPORT_VALIDATION_LAYER();
	return (true);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL	debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
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

bool	VulkanInstance::setupDebugMessenger(void) {
	VkDebugUtilsMessengerCreateInfoEXT	createInfo{};
	populateMessengerCreateInfo(createInfo);
	VkResult	res = VK_SUCCESS;
	CALL_VKINSTANCE_FUNC_VKRESULT(res, _instance, vkCreateDebugUtilsMessengerEXT,
									&createInfo, nullptr, &_debugMessenger);
	if (res != VK_SUCCESS) {
		_healthy = false;
		_reason = "Failed to create the messenger";
		return (true);
	}
	return (false);
}

}
