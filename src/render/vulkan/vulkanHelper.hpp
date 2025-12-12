/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: vulkanHelper.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/12 18:17:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/12 18:46:41                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>

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

}
