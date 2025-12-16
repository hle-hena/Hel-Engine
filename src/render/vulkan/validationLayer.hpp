/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: validationLayer.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 13:53:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/16 20:15:54                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#ifndef VALIDATION_LAYERS

	#define DESTROY_DEBUG_MESSENGER()

	#define ADD_VALIDATION_LAYER_SUPPORT()

	#define CHECK_SUPPORT_VALIDATION_LAYER()

	#define SETUP_VALIDATION_LAYER() false

	#define ADD_VALIDATION_LAYERS()

#else

	#define DESTROY_DEBUG_MESSENGER()			\
		CALL_VKINSTANCE_FUNC_VOID(				\
			_instance,							\
			vkDestroyDebugUtilsMessengerEXT,	\
			_debugMessenger,					\
			nullptr								\
		);

	#define ADD_VALIDATION_LAYER_SUPPORT()		\
		reqExt.push_back(						\
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME	\
		);

	#define CHECK_SUPPORT_VALIDATION_LAYER()						\
		auto	availableLayers = enumerate<VkLayerProperties>(		\
			ENUMERATE_WRAP(vkEnumerateInstanceLayerProperties)		\
		);															\
		if (!checkSupport("validation layer",						\
						_validationLayers,							\
						availableLayers,							\
						[](const VkLayerProperties &p){				\
							return (p.layerName);					\
						}))											\
			return (false);
	
	#define SETUP_VALIDATION_LAYER() setupDebugMessenger()

	#define ADD_VALIDATION_LAYERS()								\
		createInfo.enabledLayerCount =							\
			static_cast<uint32_t>(_validationLayers.size());	\
		createInfo.ppEnabledLayerNames =						\
			_validationLayers.data();							\
																\
		populateMessengerCreateInfo(debugInfo);					\
		createInfo.pNext = &debugInfo;

#endif
