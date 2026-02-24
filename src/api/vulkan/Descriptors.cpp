/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Descriptors.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/22 18:47:42 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/24 21:29:33                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Descriptors.hpp"
#include "api/vulkan/Device.hpp"

namespace	hel {

std::unique_ptr<DescriptorPool>	DescriptorPool::Builder::build(void) {
	std::vector<VkDescriptorPoolSize>	poolSizes{};
	for (auto it: _descriptorsRatio) {
		VkDescriptorPoolSize	push;
		push.type = it.first;
		push.descriptorCount = it.second * _pageSize;
		poolSizes.push_back(push);
	}

	return (std::make_unique<DescriptorPool>(_device, _pageSize,
											_poolCreationFlags, poolSizes));
}

DescriptorPool::DescriptorPool(Device &device, uint32_t pageSize,
							VkDescriptorPoolCreateFlags flags,
							std::vector<VkDescriptorPoolSize> &poolSizes)
	:	_device{device},
		_pageSize{pageSize},
		_poolCreationFlags{flags},
		_poolSizes{poolSizes} {
	getNewPool();
}

DescriptorPool::~DescriptorPool(void) {
	resetPools();
	for (auto &handle: _pools)
		vkDestroyDescriptorPool(_device.getLogical(), handle->_pool, nullptr);
}

void	DescriptorPool::resetPools(void) {
	for (auto handle: _fullPools) {
		vkResetDescriptorPool(_device.getLogical(), handle->_pool, 0);
		_emptyPools.push_back(handle);
	}
	_fullPools.clear();
	if (_activeHandle)
		vkResetDescriptorPool(_device.getLogical(), _activeHandle->_pool, 0);
}

void	DescriptorPool::freeSets(DescriptorSet &handle) {
	Pool		*parent = handle.parentPool;
	uint32_t	setCount = handle.sets.size();
	vkFreeDescriptorSets(_device.getLogical(), parent->_pool,
						setCount, handle.sets.data());
	parent->_allocatedSets -= setCount;
	if (!parent->_allocatedSets) {
		auto	it = std::find(_fullPools.begin(), _fullPools.end(), parent);
		if (it != _fullPools.end())	{ _fullPools.erase(it); }
		_emptyPools.push_back(parent);
	}
}

DescriptorPool::Pool::ptr	DescriptorPool::createNewPool(void) {
	Pool::ptr	newPool = std::make_unique<Pool>();
	VkDescriptorPoolCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.flags = _poolCreationFlags;
	createInfo.maxSets = _pageSize;
	createInfo.poolSizeCount = _poolSizes.size();
	createInfo.pPoolSizes = _poolSizes.data();
	
	if (vkCreateDescriptorPool(_device.getLogical(), &createInfo, nullptr,
							&newPool->_pool))
		return (nullptr);
	return (newPool);
}

void	DescriptorPool::getNewPool(void) {
	if (_activeHandle)
		_fullPools.push_back(_activeHandle);
	if (_emptyPools.empty()) {
		auto	newPool = createNewPool();
		if (!newPool) {
			_fullPools.pop_back();
			return ;
		}
		_activeHandle = newPool.get();
		_pools.push_back(std::move(newPool));
	} else {
		_activeHandle = _emptyPools.back();
		_emptyPools.pop_back();
	}
}

bool	DescriptorPool::allocateSets(VkDescriptorSetLayout setLayout,
								DescriptorSet &handle, uint32_t setCount) {
	handle.sets.resize(static_cast<size_t>(setCount));
	std::vector<VkDescriptorSetLayout>	setLayouts(setCount, setLayout);
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
