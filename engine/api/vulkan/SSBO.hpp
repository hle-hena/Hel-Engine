/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SSBO.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/10 09:24:19 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/10 12:04:01                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "api/vulkan/Swapchain.hpp"
#include "HelExpected.hpp"

#include <array>

namespace	hel {

class	Buffer;

template <typename T>
class	SSBO {
	private:
		Device			*_device;
		size_t			_count;
		std::array<std::unique_ptr<Buffer>, Swapchain::MAX_FRAMES_IN_FLIGHT>	_buffers;

		expected<void>	ensureSize(uint32_t count);
	public:
		SSBO(void) = default;
		~SSBO(void) = default;
		SSBO(const SSBO &other) = delete;
		SSBO	operator=(const SSBO &other) = delete;
		SSBO(const SSBO &&other) = delete;
		SSBO	operator=(const SSBO &&other) = delete;

		void			init(Device *device);
		expected<void>	update(const std::vector<T> &data, uint32_t frameIndex);
		VkDescriptorBufferInfo	getDescriptorInfo(uint32_t frameIndex);
};

}

#include "api/vulkan/SSBO.tpp"
