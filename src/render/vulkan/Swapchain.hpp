/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Swapchain.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 09:27:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/06 17:52:11                                        */
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

namespace	hel {

class	Device;
class	Window;


class	Swapchain
{
	public:
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

		bool	initiateSwapChain(Window &window);
		void	deleteSwapChain(void);

	private:
		VkSurfaceFormatKHR	selectSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats);
		VkPresentModeKHR	selectSwapPresent(std::vector<VkPresentModeKHR> &presents);
		VkExtent2D			selectSwapExtent(const VkSurfaceCapabilitiesKHR &presents,
													GLFWwindow *window);

		bool				createImagesView(void);

		bool						_healthy{true};
		std::string					_reason{""};
		Device						&_device;
		VkSwapchainKHR				_swapchain{VK_NULL_HANDLE};
		std::vector<VkImage>		_images;
		std::vector<VkImageView>	_imagesView;
		VkFormat					_format;
		VkExtent2D					_extent;
};

}