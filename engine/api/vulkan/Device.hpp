/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Device.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/15 10:35:22 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/27 18:16:07                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <cstdint>
#include <optional>
#include <vma/vk_mem_alloc.h>

#include "HelExpected.hpp"

namespace	hel {

class	VulkanInstance;
class	Window;

struct	QueuesFamilyIndices {
	std::optional<uint32_t>	graphicsFamily;
	std::optional<uint32_t>	presentFamily;

	bool	isComplete() const {
		return (graphicsFamily.has_value() && presentFamily.has_value());
	}
};

class	Device {
	public:
		Device(void) = default;
		~Device(void);
		Device(const Device &other) = delete;
		Device	&operator=(const Device &other) = delete;
		Device(Device &&other) = default;
		Device	&operator=(Device &&other) = delete;

		VkPhysicalDevice	&getPhysical(void) {
			return (_physicalDevice);
		}
		VkDevice			getLogical(void) {
			return (_device);
		}
		VmaAllocator		getAllocator(void) const {
			return (_allocator);
		}
		QueuesFamilyIndices	&getQueueFamily(void) {
			return (_indices);
		}
		VkQueue				getGraphicsQueue(void) const {
			return (_graphicQueue);
		}
		VkQueue				getPresentQueue(void) const {
			return (_presentQueue);
		}
		VkPhysicalDeviceProperties2	getPhysProperties(void) const {
			return (_physicalProperties);
		}
		uint32_t	getAligned(uint32_t stride, VkBufferUsageFlags usage) const;

		expected<void>		init(VulkanInstance *instance, Window *bootstrap);
		bool				supportSurface(Window *window);

		bool				findMemoryType(uint32_t typeFilter,
								VkMemoryPropertyFlags properties,
								uint32_t &outTypeIndex);

		VkCommandBuffer		beginSingleTimeCommand(void);
		void				endSingleTimeCommand(VkCommandBuffer commandBuffer);

	private:
		bool				isDeviceSuitable(VkPhysicalDevice device,
											Window *bootstrapWindow);
		bool				checkDeviceExtensionSupport(VkPhysicalDevice device);
		QueuesFamilyIndices	findQueueFamilies(VkPhysicalDevice device,
											Window *bootstrapWindow);

		expected<void>				pickPhysicalDevice(VulkanInstance *instance,
													Window *bootstrapWindow);
		expected<void>				createLogicalDevice(void);
		expected<void>				createCommandPool(void);
		expected<void>				createVmaAllocator(VulkanInstance *instance);

		VkPhysicalDevice			_physicalDevice{VK_NULL_HANDLE};
		VkPhysicalDeviceProperties2	_physicalProperties{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
														.pNext = nullptr, .properties = {}};
		VkDevice					_device{VK_NULL_HANDLE};
		QueuesFamilyIndices			_indices;
		VkQueue						_graphicQueue;
		VkQueue						_presentQueue;
		VkCommandPool				_transientCommandPool{VK_NULL_HANDLE};
		VmaAllocator				_allocator{VK_NULL_HANDLE};

		const std::vector<const char *>	_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

}
