/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Sampler.cpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/26 11:02:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/26 11:47:12                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Sampler.hpp"
#include "api/vulkan/Device.hpp"
#include "utils/mathUtils.hpp"

#include <algorithm>

namespace	std {

template<>
struct hash<hel::Sampler::Config>
{
	size_t	operator()(const hel::Sampler::Config &config) const
	{
		size_t	seed = 0;
		hel::mathUtils::hashCombine(seed, config.addresMode, config.magFilter,
				config.minFilter, config.anisotropyEnable, config.maxAnisotropy,
				config.mipmapMode, config.borderColor);
		return (seed);
	}	
};

}

namespace	hel {

bool	Sampler::Config::operator==(const Config &other) const {
	return (this->addresMode == other.addresMode &&
			this->minFilter == other.minFilter &&
			this->magFilter == other.magFilter &&
			this->anisotropyEnable == other.anisotropyEnable &&
			this->maxAnisotropy == other.maxAnisotropy &&
			this->mipmapMode == other.mipmapMode &&
			this->borderColor == other.borderColor);
}

void	Sampler::deleteAllSamplers(Device &device) {
	for (auto it: _samplers)
		vkDestroySampler(device.getLogical(), it.second, nullptr);
}

VkSampler	Sampler::getSampler(Device &device, Config config) {
	VkPhysicalDeviceProperties2	properties;
	vkGetPhysicalDeviceProperties2(device.getPhysical(), &properties);
	config.maxAnisotropy = std::min(config.maxAnisotropy,
							properties.properties.limits.maxSamplerAnisotropy);

	if (_samplers.find(config) != _samplers.end())
		return (_samplers[config]);

	VkSamplerCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.magFilter = config.magFilter;
	createInfo.minFilter = config.minFilter;
	createInfo.addressModeU = config.addresMode;
	createInfo.addressModeV = config.addresMode;
	createInfo.addressModeW = config.addresMode;
	createInfo.anisotropyEnable = config.anisotropyEnable;
	createInfo.maxAnisotropy = config.maxAnisotropy;
	createInfo.borderColor = config.borderColor;
	createInfo.unnormalizedCoordinates = VK_FALSE;
	createInfo.compareEnable = VK_FALSE;
	createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	createInfo.mipmapMode = config.mipmapMode;
	createInfo.mipLodBias = 0.0f;
	createInfo.minLod = 0.0f;
	createInfo.maxLod = VK_LOD_CLAMP_NONE;

	VkSampler	newSampler;
	if (vkCreateSampler(device.getLogical(), &createInfo, nullptr, &newSampler))
		return (nullptr);
	_samplers[config] = newSampler;
	return (newSampler);
}

}
