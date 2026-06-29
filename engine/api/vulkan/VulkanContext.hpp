/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanContext.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:31:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/26 15:01:22                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "HelExpected.hpp"
#include "api/vulkan/VulkanInstance.hpp"
#include "api/vulkan/Device.hpp"

namespace	hel {

class	VulkanContext {
	public:
		VulkanContext(void);
		~VulkanContext(void) = default;
		VulkanContext(const VulkanContext &other) = delete;
		VulkanContext	&operator=(const VulkanContext &other) = delete;
		VulkanContext(VulkanContext &&other) = default;
		VulkanContext	&operator=(VulkanContext &&other) = delete;

		VkInstance	getInstance(void)	{ return _instance.getVkInstance(); }
		Device		*getDevice(void)	{ return &_device; }

		expected<void>	initiateVulkan(void);

	private:
		VulkanInstance	_instance;
		Device			_device;
};

}
