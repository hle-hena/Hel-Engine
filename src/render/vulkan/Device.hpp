/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/11 10:10:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/12 22:39:54                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "platform/window/Window.hpp"

# include <vector>
# include <iostream>
# include <cstring>

namespace	hel {

class	Device {
	public:
		#ifdef VALIDATION_LAYERS
			static constexpr bool enableValidationLayers = true;
		#else
			static constexpr bool enableValidationLayers = false;
		#endif

		Device(void);
		~Device(void);
		Device(const Device &other) = delete;
		Device	&operator=(const Device &other) = delete;
		Device(Device &&other) = default;
		Device	&operator=(Device &&other) = default;

		std::string					getReason(void) const { return (_reason); }

		bool	isHealthy(void) const {
			return (_healthy);
		}

	private:
		template <typename T, typename Extractor>
		bool	checkSupport(const std::string &type, const std::vector<const char *> &required,
							std::vector<T> &available, Extractor extractName) {
			for (const char *reqName: required) {
				bool	found = false;
				for (const T &value: available) {
					if (std::strcmp(reqName, extractName(value)) == 0) {
						found = true;
						break ;
					}
				}
				if (!found) {
					_healthy = false;
					_reason = "Missing support for a(n) " + type + ": \"" + reqName + "\"";
					return (false);
				}
			}
			std::cout << "All " << type << " have been found" << std::endl;
			return (true);
		}

		void						createInstance(void);
		bool						checkAllSupport(std::vector<const char *> &reqExt);

		void						populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
		void						setupDebugMessenger(void);

		bool						_healthy{true};
		std::string					_reason{""};
		VkInstance					_instance;
		VkDebugUtilsMessengerEXT	_debugMessenger;

		const std::vector<const char *>	_validationLayers = { "VK_LAYER_KHRONOS_validation" };
};

}