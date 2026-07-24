/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Swapchain.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 09:27:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 10:33:11                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

#include "utils/Expected.hpp"
#include "utils/Ref.hpp"

namespace	hel {

class	Device;
class	Window;
class	Image;

class	Swapchain
{
	public:
		static constexpr int		MAX_FRAMES_IN_FLIGHT = 2;
		static constexpr VkFormat	SWAPCHAIN_FORMAT = VK_FORMAT_B8G8R8A8_UNORM;

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
		Swapchain	&operator=(Swapchain &&other) = delete;

		static SupportDetails		querySwapChainSupport(VkPhysicalDevice &device,
														VkSurfaceKHR surface);

		expected<void>	initiateSwapChain(Window &window);
		expected<void>	recreateSwapChain(Window &window);
		void			deleteSwapChain(void);

		Image			*getSwapImage(uint32_t imageIndex);
		void			waitForFrameFence(uint32_t frameIndex);
		bool			acquireNextImage(Window &window, uint32_t currentFrame, uint32_t *imageIndex);
		bool			submitCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame);
		bool			present(Window &window, uint32_t imageIndex);

		bool	_frameBufferResized{false};

	private:
		VkSurfaceFormatKHR	selectSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats);
		VkPresentModeKHR	selectSwapPresent(std::vector<VkPresentModeKHR> &presents);
		VkExtent2D			selectSwapExtent(const VkSurfaceCapabilitiesKHR &presents,
													GLFWwindow *window);

		expected<void>		createSyncObjects(void);

		Device								&_device;
		VkSwapchainKHR						_swapchain{VK_NULL_HANDLE};
		std::vector<Ref<Image>>	_swapImages;
		std::vector<VkSemaphore>			_imageAvailable{VK_NULL_HANDLE};
		std::vector<VkSemaphore>			_renderFinished{VK_NULL_HANDLE};
		std::array<VkFence,		MAX_FRAMES_IN_FLIGHT>	_inFlightFences{VK_NULL_HANDLE};
};

}