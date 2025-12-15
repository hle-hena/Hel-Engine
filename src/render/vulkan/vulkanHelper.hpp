/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: vulkanHelper.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/12 18:17:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/15 13:49:45                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <utility>

namespace	hel {

#define CALL_VKINSTANCE_FUNC_VKRESULT(resultVar, instance, funcName, ...)		\
do {																			\
	auto	func = (PFN_##funcName) vkGetInstanceProcAddr(instance, #funcName);	\
	if (func) resultVar = (func(instance, __VA_ARGS__));						\
	else resultVar = (VK_ERROR_EXTENSION_NOT_PRESENT);							\
} while (0)

#define CALL_VKINSTANCE_FUNC_VOID(instance, funcName, ...)						\
do {																			\
	auto	func = (PFN_##funcName) vkGetInstanceProcAddr(instance, #funcName);	\
	if (func) func(instance, __VA_ARGS__);										\
} while (0)

#define A(funcName)			\
do {						\
	uint32_t	count = 0;	\
							\
}


#define ENUMERATE_WRAP(funcName, ...)		\
[&](uint32_t *c, auto *d){					\
	return (funcName(__VA_OPT__(__VA_ARGS__, ) c, d));	\
}

template <typename T, typename ExtractorFunc>
std::vector<T>	enumerate(ExtractorFunc &&f) {
	uint32_t	count = 0;
	f(&count, static_cast<T *>(nullptr));
	std::vector<T>	out(count);
	f(&count, out.data());
	return (out);
}

/* 

	uint32_t	physDeviceCount = 0;
	vkEnumeratePhysicalDevices(_instance.getVkInstance(), &physDeviceCount, nullptr);
	if (physDeviceCount == 0) {
		_healthy = false;
		_reason = "Couldn't find a GPU to pair with vulkan";
		return (true);
	}
	std::vector<VkPhysicalDevice>	physDevices(physDeviceCount);
	vkEnumeratePhysicalDevices(_instance.getVkInstance(), &physDeviceCount, physDevices.data());


	uint32_t	glfwExtensionsCount = 0;
	const char	**glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	std::vector<const char *>	reqExt(glfwExtensions, glfwExtensions + glfwExtensionsCount);

	uint32_t	avExtCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &avExtCount, nullptr);
	std::vector<VkExtensionProperties>	avExt(avExtCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &avExtCount, avExt.data());

*/

}
