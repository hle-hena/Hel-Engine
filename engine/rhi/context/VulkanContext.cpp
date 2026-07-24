/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanContext.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:32:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 17:29:28                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/context/VulkanContext.hpp"
#include "rhi/window/Window.hpp"
#include "rhi/render/Shader.hpp"

namespace	hel {

VulkanContext::VulkanContext() {}
VulkanContext::~VulkanContext() {
	ShaderCache::clear();
}

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
	ShaderCache::init(&_device);
	return {};
}

}
