/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanInstance.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:33:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/05 18:15:37                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>
# include <string>
# include <cstring>
# include <vector>
# include <iostream>

# include "utils/healthHelper.hpp"

namespace hel {

class	VulkanInstance {
	public:
		VulkanInstance(void) = default;
		~VulkanInstance(void);
		VulkanInstance(const VulkanInstance &other) = delete;
		VulkanInstance	&operator=(const VulkanInstance &other) = delete;
		VulkanInstance(VulkanInstance &&other) = default;
		VulkanInstance	&operator=(VulkanInstance &&other) = default;

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}
		VkInstance		&getVkInstance(void) {
			return (_instance);
		}

		bool			createInstance(void);

	private:
		template <typename T, typename Extractor>
		bool	checkSupport(const std::string &type, const std::vector<const char *> &required,
							std::vector<T> &available, Extractor &&extractName) {
			for (const char *reqName: required) {
				bool	found = false;
				for (const T &value: available) {
					if (std::strcmp(reqName, extractName(value)) == 0) {
						found = true;
						break ;
					}
				}
				if (!found)
					RETURN_SET_UNHEALTHY(
						"Missing support for a(n) " + type + ": \"" + reqName + "\"",
						true
					);
			}
			std::cout << "All " << type << " have been found" << std::endl;
			return (false);
		}

		std::vector<const char *>	getExtensions(void);
		bool						checkAllSupport(std::vector<const char *> &reqExt);

		bool						setupDebugMessenger(void);
		void						populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

		bool							_healthy{true};
		std::string						_reason{""};
		VkInstance						_instance{VK_NULL_HANDLE};
		VkDebugUtilsMessengerEXT		_debugMessenger{VK_NULL_HANDLE};

		const std::vector<const char *>	_validationLayers = { "VK_LAYER_KHRONOS_validation" };
};

}
