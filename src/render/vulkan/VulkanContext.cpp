/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanContext.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:32:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/16 20:28:07                                        */
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
	:	_device{_instance} {
}

bool	VulkanContext::initiateVulkan(void) {
	if (_instance.createInstance())
		RETURN_SET_UNHEALTHY(_instance.getReason(), true);
	if (_device.pickPhysicalDevice())
		RETURN_SET_UNHEALTHY(_device.getReason(), true);
	return (false);
}

}
