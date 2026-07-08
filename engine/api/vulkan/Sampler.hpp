/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Sampler.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/26 11:02:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/05 16:30:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <unordered_map>
#include <vulkan/vulkan.h>

namespace	hel {

class	Device;

class	Sampler {
	public:
		struct	Config {
			VkFilter				magFilter{VK_FILTER_LINEAR};
			VkFilter				minFilter{VK_FILTER_LINEAR};
			VkSamplerAddressMode	addresMode{VK_SAMPLER_ADDRESS_MODE_REPEAT};
			VkBool32				anisotropyEnable{VK_TRUE};
			float					maxAnisotropy = 16.0f;
			VkSamplerMipmapMode		mipmapMode{VK_SAMPLER_MIPMAP_MODE_LINEAR};
			VkBorderColor			borderColor{VK_BORDER_COLOR_INT_OPAQUE_BLACK};

			bool	operator==(const Config &other) const;
		};
		~Sampler(void) = delete;

		static void			deleteAllSamplers(Device &device);
		static VkSampler	getSampler(Device &device, Config config);

	private:
		static std::unordered_map<Config, VkSampler>	_samplers;
};

}
