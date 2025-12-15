/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanContext.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:31:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/15 11:23:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "render/vulkan/VulkanInstance.hpp"
# include "render/vulkan/PhysicalDevice.hpp"

namespace	hel {

class	VulkanContext {
	public:
		VulkanContext(void);
		~VulkanContext(void) = default;
		VulkanContext(const VulkanContext &other) = delete;
		VulkanContext	&operator=(const VulkanContext &other) = delete;
		VulkanContext(VulkanContext &&other) = default;
		VulkanContext	&operator=(VulkanContext &&other) = default;

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}

	private:
		bool			_healthy{true};
		std::string		_reason{""};
		VulkanInstance	_instance;
		PhysicalDevice	_physicalDevice;
};

}
