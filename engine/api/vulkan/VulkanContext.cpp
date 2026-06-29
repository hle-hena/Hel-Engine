/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanContext.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:32:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/26 15:26:43                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/VulkanContext.hpp"
#include "platform/window/Window.hpp"

namespace	hel {

VulkanContext::VulkanContext()
{}

expected<void>	VulkanContext::initiateVulkan(void) {
	{
	auto	res = _instance.createInstance();
	if (!res)
		return unexpected(res.error());
	}

	Window::windowPtr	bootstrapWindow = Window::createBootstrap(600, 600, "bootstrap", this);
	if (!bootstrapWindow)
		return unexpected("Couldn't create the bootstrap window");

	{
	auto	res = _device.init(&_instance, bootstrapWindow.get());
	if (!res)
		return unexpected(res.error());
	}
	return {};
}

}
