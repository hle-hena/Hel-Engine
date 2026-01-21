/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:22 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 20:53:58                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vector>
# include <vulkan/vulkan.h>
# include <cstdint>
# include <string>
# include <optional>

namespace	hel {

class	Window;
class	VulkanInstance;

struct	QueuesFamilyIndices {
	std::optional<uint32_t>	graphicsFamily;
	std::optional<uint32_t>	presentFamily;

	bool	isComplete() const {
		return (graphicsFamily.has_value() && presentFamily.has_value());
	}
};

class	Device {
	public:
		Device(VulkanInstance	&instance);
		~Device(void);
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
		VkPhysicalDevice	&getPhysical(void) {
			return (_physicalDevice);
		}
		VkDevice			getLogical(void) {
			return (_device);
		}
		QueuesFamilyIndices	&getQueueFamily(void) {
			return (_indices);
		}
		VkQueue	getGraphicsQueue(void) const {
			return (_graphicQueue);
		}
		VkQueue	getPresentQueue(void) const {
			return (_presentQueue);
		}

		bool	pickPhysicalDevice(Window &bootstrapWindow);
		bool	supportSurface(Window &window);

	private:
		bool				isDeviceSuitable(VkPhysicalDevice device, Window &bootstrapWindow);
		bool				checkDeviceExtensionSupport(VkPhysicalDevice device);
		QueuesFamilyIndices	findQueueFamilies(VkPhysicalDevice device, Window &bootstrapWindow);

		bool				createLogicalDevice();

		bool				_healthy{true};
		std::string			_reason{""};
		VulkanInstance		&_instance;
		VkPhysicalDevice	_physicalDevice{VK_NULL_HANDLE};
		VkDevice			_device{VK_NULL_HANDLE};
		QueuesFamilyIndices	_indices;
		VkQueue				_graphicQueue;
		VkQueue				_presentQueue;

		const std::vector<const char *>	_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

}
