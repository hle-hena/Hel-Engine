/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: PhysicalDevice.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:22 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/15 11:20:00                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "render/vulkan/VulkanInstance.hpp"

namespace	hel {

class	PhysicalDevice {
	public:
		PhysicalDevice(VulkanInstance	&instance);
		~PhysicalDevice(void) = default;
		PhysicalDevice(const PhysicalDevice &other) = delete;
		PhysicalDevice	&operator=(const PhysicalDevice &other) = delete;
		PhysicalDevice(PhysicalDevice &&other) = default;
		PhysicalDevice	&operator=(PhysicalDevice &&other) = default;

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}

		bool	pickPhysicalDevice(void);

	private:
		bool				_healthy{true};
		std::string			_reason{""};
		VulkanInstance		&_instance;
		VkPhysicalDevice	_device{VK_NULL_HANDLE};

		bool	isDeviceSuitable(VkPhysicalDevice device);
};

}
