/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/11 10:10:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/12 18:13:14                                        */
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
		void						createInstance(void);
		std::vector<const char *>	getExtensions(void);
		bool						checkExtensionSupport(std::vector<const char *> &extensions);
		bool						checkValidationLayerSupport(void);
		void						populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
		void						setupDebugMessenger(void);

		bool						_healthy{true};
		std::string					_reason{""};
		VkInstance					_instance;
		VkDebugUtilsMessengerEXT	_debugMessenger;

		const std::vector<const char *>	_validationLayers = { "VK_LAYER_KHRONOS_validation" };
};

}