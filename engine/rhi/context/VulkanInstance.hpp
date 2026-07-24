/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VulkanInstance.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:33:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 10:33:27                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>

#include "utils/Expected.hpp"

namespace hel {

class	VulkanInstance {
	public:
		VulkanInstance(void) = default;
		~VulkanInstance(void);
		VulkanInstance(const VulkanInstance &other) = delete;
		VulkanInstance	&operator=(const VulkanInstance &other) = delete;
		VulkanInstance(VulkanInstance &&other) = default;
		VulkanInstance	&operator=(VulkanInstance &&other) = delete;

		VkInstance		&getVkInstance(void) {
			return (_instance);
		}

		expected<void>	createInstance(void);

	private:
		template <typename T, typename Extractor>
		expected<void>	checkSupport(const std::string &type, const std::vector<const char *> &required,
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
					return unexpected("Missing support for a(n) " + type
									+ ": \"" + reqName + "\"");
			}
			std::cout << "All " << type << " have been found" << std::endl;
			return {};
		}

		std::vector<const char *>	getExtensions(void);
		bool						checkAllSupport(std::vector<const char *> &reqExt);

		expected<void>				setupDebugMessenger(void);
		void						populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

		VkInstance						_instance{VK_NULL_HANDLE};
		VkDebugUtilsMessengerEXT		_debugMessenger{VK_NULL_HANDLE};

		const std::vector<const char *>	_validationLayers = { "VK_LAYER_KHRONOS_validation" };
};

}
