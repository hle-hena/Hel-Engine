/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Swapchain.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 09:27:33 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/04 19:21:17                                        */
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
		VkFormat		getFormat(void) const {
			return (_format);
		}
		VkFormat		getDepthFormat(void) const {
			return (_depthFormat);
		}
		VkExtent2D		getExtent(void) const {
			return (_extent);
		}

		static SupportDetails		querySwapChainSupport(VkPhysicalDevice &device,
														VkSurfaceKHR surface);

		bool			initiateSwapChain(Window &window);
		bool			recreateSwapChain(Window &window);
		VkFramebuffer	getFrameBuffer(uint32_t imageIndex, VkRenderPass renderPass);
		void			deleteSwapChain(void);

		bool			acquireNextImage(Window &window, uint32_t currentFrame, uint32_t *imageIndex);
		bool			submitCommandBuffer(VkCommandBuffer *commandBuffer,
									uint32_t imageIndex, uint32_t currentFrame);
		bool			present(Window &window, uint32_t imageIndex, uint32_t currentFrame);

		bool	_frameBufferResized{false};

	private:
		using framebuffersMap = std::unordered_map<VkRenderPass, std::vector<VkFramebuffer>>;

		VkSurfaceFormatKHR	selectSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats);
		VkPresentModeKHR	selectSwapPresent(std::vector<VkPresentModeKHR> &presents);
		VkExtent2D			selectSwapExtent(const VkSurfaceCapabilitiesKHR &presents,
													GLFWwindow *window);
		VkFormat			selectDepthFormat(const std::vector<VkFormat> &candidates,
										VkImageTiling tiling,
										VkFormatFeatureFlags features);

		bool				createImage(VkImage &image, VkDeviceMemory &memory,
										VkExtent3D extent, VkFormat format,
										VkImageTiling tiling, VkImageUsageFlags usage,
										VkMemoryPropertyFlags properties);
		bool				createImageView(VkImage &image, VkImageView &imageView,
										VkFormat format, VkImageAspectFlags aspectFlag);

		bool				createSwapchainImageView(void);
		bool				createFramebuffersForRenderPass(VkRenderPass renderPass);
		bool				createSyncObjects(void);
		bool				createDepthResources(void);

		bool						_healthy{true};
		std::string					_reason{""};
		Device						&_device;
		VkSwapchainKHR				_swapchain{VK_NULL_HANDLE};
		std::vector<VkImage>		_images;
		std::vector<VkImageView>	_imagesView;
		VkFormat					_depthFormat;
		VkImage						_depthImage{VK_NULL_HANDLE};
		VkDeviceMemory				_depthImageMemory{VK_NULL_HANDLE};
		VkImageView					_depthImageView{VK_NULL_HANDLE};
		framebuffersMap				_frameBufferCache;
		VkFormat					_format;
		VkExtent2D					_extent;
		std::array<VkSemaphore,	MAX_FRAMES_IN_FLIGHT>	_imageAvailable{VK_NULL_HANDLE};
		std::array<VkSemaphore,	MAX_FRAMES_IN_FLIGHT>	_renderFinished{VK_NULL_HANDLE};
		std::array<VkFence,		MAX_FRAMES_IN_FLIGHT>	_inFlightFences{VK_NULL_HANDLE};
};

}