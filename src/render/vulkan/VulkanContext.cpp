/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanContext.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:32:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/05 16:29:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "render/vulkan/VulkanContext.hpp"
#include "platform/window/Window.hpp"
#include "platform/window/GLFW.hpp"

namespace	hel {

VulkanContext::VulkanContext(Application &app)
	:	_device{_instance},
		_app{app} {
}

bool	VulkanContext::initiateVulkan(void) {
	if (!GLFW::acquire())
        RETURN_SET_UNHEALTHY("GLFW init failed", true);

	if (_instance.createInstance()) {
		GLFW::release();
		RETURN_SET_UNHEALTHY(_instance.getReason(), true);
	}
	GLFW::release();

	Window::windowPtr bootstrapWindow = Window::createWindow(600, 600, "bootstrap",
		_app, _instance.getVkInstance());
	if (!bootstrapWindow)
		RETURN_SET_UNHEALTHY("Couldn't create the bootstrap window", true);

	if (_device.pickPhysicalDevice(bootstrapWindow))
		RETURN_SET_UNHEALTHY(_device.getReason(), true);
	return (false);
}

}
