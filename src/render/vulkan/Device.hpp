/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:22 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/15 15:08:16                                        */
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

class	Device {
	public:
		Device(VulkanInstance	&instance);
		~Device(void) = default;
		Device(const Device &other) = delete;
		Device	&operator=(const Device &other) = delete;
		Device(Device &&other) = default;
		Device	&operator=(Device &&other) = default;

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
