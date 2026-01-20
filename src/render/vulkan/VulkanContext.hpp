/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanContext.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:31:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/19 17:26:29                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "render/vulkan/VulkanInstance.hpp"
# include "render/vulkan/Device.hpp"

namespace	hel {

class	Application;

class	VulkanContext {
	public:
		VulkanContext(Application &app);
		~VulkanContext(void) = default;
		VulkanContext(const VulkanContext &other) = delete;
		VulkanContext	&operator=(const VulkanContext &other) = delete;
		VulkanContext(VulkanContext &&other) = default;
		VulkanContext	&operator=(VulkanContext &&other) = default;

		VulkanInstance	&getInstance(void) {
			return (_instance);
		}
		Device			&getDevice(void) {
			return (_device);
		}
		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}

		bool	initiateVulkan(void);

	private:
		bool			_healthy{true};
		std::string		_reason{""};
		VulkanInstance	_instance;
		Device			_device;
		Application		&_app;
};

}
