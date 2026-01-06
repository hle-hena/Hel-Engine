/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SwapChain.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 09:27:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/06 16:34:37                                        */
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


class	SwapChain
{
	public:
		struct	SupportDetails {
			VkSurfaceCapabilitiesKHR		capabilities;
			std::vector<VkSurfaceFormatKHR>	formats;
			std::vector<VkPresentModeKHR>	presents;
		};

		SwapChain(Device &device);
		~SwapChain(void);
		SwapChain(const SwapChain &other) = delete;
		SwapChain	&operator=(const SwapChain &other) = delete;
		SwapChain(SwapChain &&other) = default;
		SwapChain	&operator=(SwapChain &&other) = default;

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

		bool					_healthy{true};
		std::string				_reason{""};
		Device					&_device;
		VkSwapchainKHR			_swapChain{VK_NULL_HANDLE};
		std::vector<VkImage>	_images;
		VkFormat				_format;
		VkExtent2D				_extent;
};

}