/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Swapchain.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 09:27:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/04 18:39:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>
# include <vector>
# include <string>
# include <unordered_map>
# include <array>

# include "api/vulkan/Image.hpp"

namespace	hel {

class	Device;
class	Window;


class	Swapchain
{
	public:
		static constexpr int	MAX_FRAMES_IN_FLIGHT = 2;

		struct	SupportDetails {
			VkSurfaceCapabilitiesKHR		capabilities;
			std::vector<VkSurfaceFormatKHR>	formats;
			std::vector<VkPresentModeKHR>	presents;
		};

		Swapchain(Device &device);
		~Swapchain(void);
		Swapchain(const Swapchain &other) = delete;
		Swapchain	&operator=(const Swapchain &other) = delete;
		Swapchain(Swapchain &&other) = default;
		Swapchain	&operator=(Swapchain &&other) = default;

		std::string		getReason(void) const {
			return (_reason);
		}
		bool			isHealthy(void) const {
			return (_healthy);
		}

		static SupportDetails		querySwapChainSupport(VkPhysicalDevice &device,
														VkSurfaceKHR surface);

		bool			initiateSwapChain(Window &window);
		bool			recreateSwapChain(Window &window);
		void			deleteSwapChain(void);

		Image			*getDepthImage(void);
		Image			*getNextColorImage(uint32_t imageIndex);
		bool			acquireNextImage(Window &window, uint32_t currentFrame, uint32_t *imageIndex);
		bool			submitCommandBuffer(VkCommandBuffer *commandBuffer,
									uint32_t imageIndex, uint32_t currentFrame);
		bool			present(Window &window, uint32_t imageIndex, uint32_t currentFrame);

		bool	_frameBufferResized{false};

	private:
		VkSurfaceFormatKHR	selectSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats);
		VkPresentModeKHR	selectSwapPresent(std::vector<VkPresentModeKHR> &presents);
		VkExtent2D			selectSwapExtent(const VkSurfaceCapabilitiesKHR &presents,
													GLFWwindow *window);
		VkFormat			selectDepthFormat(const std::vector<VkFormat> &candidates,
										VkImageTiling tiling,
										VkFormatFeatureFlags features);

		bool	createSwapchainImageViews(std::vector<VkImage> &images,
									VkFormat format, VkExtent2D extent);
		bool	createDepthResources(VkExtent2D extent);
		bool	createSyncObjects(void);

		bool						_healthy{true};
		std::string					_reason{""};
		Device						&_device;
		VkSwapchainKHR				_swapchain{VK_NULL_HANDLE};
		std::vector<Image::ptr>		_colorImages;
		Image::ptr					_depthImage;
		std::array<VkSemaphore,	MAX_FRAMES_IN_FLIGHT>	_imageAvailable{VK_NULL_HANDLE};
		std::array<VkSemaphore,	MAX_FRAMES_IN_FLIGHT>	_renderFinished{VK_NULL_HANDLE};
		std::array<VkFence,		MAX_FRAMES_IN_FLIGHT>	_inFlightFences{VK_NULL_HANDLE};
};

}