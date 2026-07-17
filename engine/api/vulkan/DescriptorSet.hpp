/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: DescriptorSet.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/17 14:49:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 15:13:33                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "utils/Ref.hpp"
#include "api/vulkan/DescriptorPool.hpp"

namespace	hel {

class	DescriptorSet : public RefCounted {
	private:
		VkDescriptorSetLayout			_setLayout;
		std::vector<VkDescriptorSet>	_sets;
		Ref<DescriptorPool>				_poolManager;

	public:
		struct	Signature;

		VkDescriptorSet	getSet(uint32_t setIndex = 0);

	friend class	DescriptorPool;
};

struct	DescriptorSet::Signature {
	private:
		std::vector<Ref<Buffer>>	_buffers;
		std::vector<Image *>		_images;
	public:
		
};

}
