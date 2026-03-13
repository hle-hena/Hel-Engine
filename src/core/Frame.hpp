/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Frame.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 15:47:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 19:51:05                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <array>
# define GLM_FORCE_RADIANS
# define GLM_FORCE_DEPTH_ZERO_TO_ONE
# include <glm/glm.hpp>

# include "api/vulkan/Swapchain.hpp"
# include "api/vulkan/Buffer.hpp"
# include "api/vulkan/Descriptors.hpp"
# include "utils/Expected.hpp"

namespace	hel {

class	Window;

struct	GlobalUBO {
	glm::mat4	viewProjection;
	float		elapsedTime;
};

struct	FrameContext {
	Window					*window{nullptr};
	VkCommandBuffer			commandBuffer;
	VkDescriptorSet			globalSet;
	VkDescriptorSetLayout	globalLayout;
	float					deltaTime{0.f};
};



class	Frame {
	public:
		Frame(void) = default;
		~Frame(void) = default;

		expected<void, std::string>	init(Device &device, DescriptorPool *pool,
										VkCommandPool commandPool);

		FrameContext	getContext(Window *window, uint32_t frameIndex,
								float deltaTime);

	private:
		std::array<VkCommandBuffer,
				Swapchain::MAX_FRAMES_IN_FLIGHT>	_commandBuffers{};
		std::array<std::unique_ptr<Buffer>,
				Swapchain::MAX_FRAMES_IN_FLIGHT>	_globalUbos{};
		std::unique_ptr<DescriptorSet>				_descriptorSets{};
};

}
