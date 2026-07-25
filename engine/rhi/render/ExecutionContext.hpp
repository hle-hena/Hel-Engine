/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ExecutionContext.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/25 17:04:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/25 17:09:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

#include "rhi/window/Window.hpp"
#include "rhi/resources/Descriptors.hpp"

namespace	hel {

struct	GlobalData;
struct	RenderRequest;

struct	ExecutionContext {
	ExecutionContext(uint32_t frameIndex, GlobalData *globalData)
		:	globals(globalData) {
		this->frameIndex = frameIndex;
	};

	Window					*window{nullptr};
	RenderRequest			*request{nullptr};

	VkCommandBuffer			commandBuffer{VK_NULL_HANDLE};

	GlobalData				*globals;
	VkDescriptorSet			globalSet{VK_NULL_HANDLE};
	uint32_t				setStride{0};
	VkDescriptorSetLayout	globalLayout{VK_NULL_HANDLE};

	DescriptorPool			*descriptorPool{nullptr};

	uint32_t				passIndex{0};
	uint32_t				frameIndex{0};
	uint32_t				swapIndex{0};
};

}
