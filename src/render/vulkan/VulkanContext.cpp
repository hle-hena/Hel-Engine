/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanContext.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:32:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/15 13:26:29                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/vulkan/VulkanContext.hpp"

namespace	hel {

VulkanContext::VulkanContext(void)
	:	_physicalDevice{_instance} {
	if (_instance.createInstance()) {
		_healthy = false;
		_reason = _instance.getReason();
		return ;
	}
	if (_physicalDevice.pickPhysicalDevice()) {
		_healthy = false;
		_reason = _physicalDevice.getReason();
	}
}

}
