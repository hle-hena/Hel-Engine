/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: DescriptorPool.cpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/16 18:12:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/17 14:58:28                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/DescriptorPool.hpp"
#include "api/vulkan/DescriptorSet.hpp"
#include "api/vulkan/Device.hpp"

namespace	hel {

DescriptorPool::DescriptorPool(Device *device, bool staticPool,
	const DescriptorPool::Signature &signature)
	:	_device(device),
		_staticPool(staticPool),
		_signature(signature)
{
	_flags = staticPool ? VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0;
}

expected<Ref<DescriptorPool::SubPool>>	DescriptorPool::createPool(void) {
	VkDescriptorPoolCreateInfo	create{};
	create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create.flags = _flags;
	create.poolSizeCount = static_cast<uint32_t>(_signature._sizes.size());
	create.pPoolSizes = _signature._sizes.data();
	create.maxSets = 512;

	auto	newPool = makeRef<DescriptorPool::SubPool>();

	if (vkCreateDescriptorPool(_device->getLogical(), &create,
							nullptr, &newPool->pool) != VK_SUCCESS)
		return unexpected("Couldn't create a new pool.");
	_emptyPools.push_back(newPool);
	return {newPool};
}

expected<Ref<DescriptorPool::SubPool>>	DescriptorPool::getActivePool(void) {
	if (_emptyPools.empty()) {
		auto	newPool = createPool();
		if (!newPool)
			return unexpected("Failed to get the active pool: "
							+ newPool.error());
		return *newPool;
	}
	return _emptyPools.back();
}

expected<Ref<DescriptorSet>>	DescriptorPool::getSet(void) {
	Ref<DescriptorPool::SubPool>	activePool;
	if (auto res = getActivePool(); !res)
		return unexpected("Couldn't get a set: " + res.error());
	

	auto	newSet = makeRef<DescriptorSet>();
	newSet->_sets.reserve(setCount);
	std::vector<VkDescriptorSetLayout>	setLayouts(setCount, newSet->_setLayout);
	VkDescriptorSetAllocateInfo	allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = (*activePool)->pool;
	allocInfo.descriptorSetCount = setCount;
	allocInfo.pSetLayouts = setLayouts.data();

	handle.sets.resize(static_cast<size_t>(setCount));
	std::vector<VkDescriptorSetLayout>	setLayouts(setCount, handle.setLayout);
	VkDescriptorSetAllocateInfo	allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = _activeHandle->_pool;
	allocateInfo.descriptorSetCount = setCount;
	allocateInfo.pSetLayouts = setLayouts.data();
	VkResult	result = vkAllocateDescriptorSets(_device.getLogical(),
											&allocateInfo, handle.sets.data());

	if (result == VK_ERROR_OUT_OF_POOL_MEMORY ||
		result == VK_ERROR_FRAGMENTED_POOL) {
		getNewPool();
		allocateInfo.descriptorPool = _activeHandle->_pool;
		result = vkAllocateDescriptorSets(_device.getLogical(),
									&allocateInfo, handle.sets.data());
	}
	if (result == VK_SUCCESS) {
		_activeHandle->_allocatedSets += setCount;
		handle.parentPool = _activeHandle;
		handle.manager = this;
		return (false);
	}
	return (true);
}

}
