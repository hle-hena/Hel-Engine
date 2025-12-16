/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanContext.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:31:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/16 20:21:58                                        */
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

		bool	initiateVulkan(void);

	private:
		bool			_healthy{true};
		std::string		_reason{""};
		VulkanInstance	_instance;
		Device			_device;
};

}
