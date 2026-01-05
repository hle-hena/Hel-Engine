/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:22 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/05 16:48:14                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "render/vulkan/VulkanInstance.hpp"
# include "platform/window/Window.hpp"
# include <optional>

namespace	hel {

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

		bool	pickPhysicalDevice(Window::windowPtr &bootstrapWindow);
		bool	supportSurface(Window::windowPtr &window);

	private:
		bool				_healthy{true};
		std::string			_reason{""};
		VulkanInstance		&_instance;
		VkPhysicalDevice	_physicalDevice{VK_NULL_HANDLE};
		VkDevice			_device{VK_NULL_HANDLE};
		QueuesFamilyIndices	_indices;
		VkQueue				_graphicQueue;
		VkQueue				_presentQueue;

		bool				isDeviceSuitable(VkPhysicalDevice device, Window::windowPtr &bootstrapWindow);
		QueuesFamilyIndices	findQueueFamilies(VkPhysicalDevice device, Window::windowPtr &bootstrapWindow);

		bool				createLogicalDevice();
};

}
