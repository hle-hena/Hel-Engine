/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: DescriptorPool.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/16 18:05:55 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 14:55:05                                        */
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
#include "HelExpected.hpp"

namespace	hel {

class	DescriptorSet;
class	Device;

class	DescriptorPool: public RefCounted {
	public:
		struct	SubPool: public RefCounted {
			VkDescriptorPool	pool{VK_NULL_HANDLE};
			uint32_t			allocatedCount{0};
		};

		expected<Ref<DescriptorSet>>	getSet(void);
		expected<Ref<SubPool>>			getActivePool(void);

	private:
		struct	Signature {
			std::vector<VkDescriptorPoolSize>	_sizes;
		};

		Device										*_device;
		bool										_staticPool;
		VkDescriptorPoolCreateFlags					_flags;
		DescriptorPool::Signature					_signature;
		std::vector<Ref<DescriptorPool::SubPool>>	_emptyPools;
		std::vector<Ref<DescriptorPool::SubPool>>	_fullPools;


		DescriptorPool(Device *device, bool staticPool,
			const DescriptorPool::Signature &signature);//AAAAAAAA

		expected<Ref<DescriptorPool::SubPool>>	createPool(void);

	friend class	DescriptorSet;
};

}
