/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Descriptors.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/22 18:47:42 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/09 10:18:51                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Descriptors.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Image.hpp"
#include "utils/mathUtils.hpp"

#include <algorithm>

namespace	std {

template <>
struct hash<hel::DescriptorBindings>
{
	size_t	operator()(const hel::DescriptorBindings &bindings) const
	{
		size_t	seed = 0;
		for (const auto &binding: bindings._bindings) {
			hel::mathUtils::hashCombine(seed, binding.binding,
											binding.descriptorType,
											binding.descriptorCount);
		}
		return (seed);
	}	
};

}

namespace	hel {

std::unordered_map<DescriptorBindings, VkDescriptorSetLayout>
	DescriptorFactory::_descriptorSetLayouts;

std::unique_ptr<DescriptorPool>	DescriptorPool::Builder::build(void) {
	std::vector<VkDescriptorPoolSize>	poolSizes{};
	for (auto it: _descriptorsRatio) {
		VkDescriptorPoolSize	push;
		push.type = it.first;
		push.descriptorCount = std::max(1u, static_cast<uint32_t>(
											it.second * static_cast<float>(_pageSize)));
		poolSizes.push_back(push);
	}

	return (std::unique_ptr<DescriptorPool>(new DescriptorPool(_device, _pageSize,
											_poolCreationFlags, poolSizes)));
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
	if (!(_poolCreationFlags & VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT))
		return ;
	Pool		*parent = handle.parentPool;
	uint32_t	setCount = static_cast<uint32_t>(handle.sets.size());
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
	createInfo.poolSizeCount = static_cast<uint32_t>(_poolSizes.size());
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

bool	DescriptorPool::allocateSets(DescriptorSet &handle, uint32_t setCount) {
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



bool	DescriptorBindings::operator==(const DescriptorBindings &other) const {
	if (other._bindings.size() != _bindings.size())
		return (false);
	for (size_t i = 0; i < _bindings.size(); i++) {
		if (_bindings[i].binding != other._bindings[i].binding ||
			_bindings[i].descriptorType != other._bindings[i].descriptorType ||
			_bindings[i].descriptorCount != other._bindings[i].descriptorCount ||
			_bindings[i].stageFlags != other._bindings[i].stageFlags)
			return (false);
	}
	return (true);
}

DescriptorFactory::DescriptorFactory(Device &device)
	:	_device{device} {
}

DescriptorFactory	&DescriptorFactory::addBinding(uint32_t binding,
												VkDescriptorType type,
												VkShaderStageFlags stages,
												VkSampler sampler,
												uint32_t descriptorCount) {
	VkDescriptorSetLayoutBinding	layoutBinding{};
	layoutBinding.binding = binding;
	layoutBinding.stageFlags = stages;
	layoutBinding.descriptorCount = descriptorCount;
	layoutBinding.descriptorType = type;
	if (sampler != VK_NULL_HANDLE) {
		_bindings._samplers.push_back(sampler);
		layoutBinding.pImmutableSamplers = &_bindings._samplers.back();
	}

	_bindings._bindings.push_back(layoutBinding);
	return (*this);
}

void	DescriptorFactory::deleteLayoutCache(Device &device) {
	for (auto it: _descriptorSetLayouts)
		vkDestroyDescriptorSetLayout(device.getLogical(), it.second, nullptr);
}

VkDescriptorSetLayout	DescriptorFactory::getSetLayout(void) {
	if (_descriptorSetLayouts.find(_bindings) != _descriptorSetLayouts.end())
		return (_descriptorSetLayouts[_bindings]);

	VkDescriptorSetLayout			newSetLayout;
	VkDescriptorSetLayoutCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = static_cast<uint32_t>(_bindings._bindings.size());
	createInfo.pBindings = _bindings._bindings.data();
	if (vkCreateDescriptorSetLayout(_device.getLogical(), &createInfo, nullptr,
									&newSetLayout))
		return (nullptr);
	_descriptorSetLayouts[_bindings] = newSetLayout;
	return (newSetLayout);
}

std::unique_ptr<DescriptorSet>	DescriptorFactory::build(
									DescriptorPool &buildPool) {
	std::sort(_bindings._bindings.begin(), _bindings._bindings.end(), 
			[](const auto& a, const auto& b) { return a.binding < b.binding; });
	VkDescriptorSetLayout	setLayout = getSetLayout();
	if (!setLayout)
		return (nullptr);
	auto	newSet = std::make_unique<DescriptorSet>();
	newSet->setLayout = setLayout;
	if (buildPool.allocateSets(*newSet, _setCount))
		return (nullptr);
	return (newSet);
}



DescriptorWriter::DescriptorWriter(Device &device, DescriptorSet *handle)
	:	_device{device},
		_handle{handle} {
}

DescriptorWriter	&DescriptorWriter::writeBuffer(uint32_t setIndex,
												uint32_t binding,
												VkDescriptorType type,
												Buffer &buffer) {
	VkDescriptorBufferInfo	bufferInfo = buffer.getDescriptorInfo();
	_buffersInfo.push_back(bufferInfo);

	VkWriteDescriptorSet	write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = _handle->sets[setIndex];
	write.dstBinding = binding;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pBufferInfo = &_buffersInfo.back();
	_writes.push_back(write);
	return (*this);
}

DescriptorWriter	&DescriptorWriter::writeImage(uint32_t setIndex,
												uint32_t binding,
												VkDescriptorType type,
												VkImageView view,
												VkImageLayout layout,
												VkSampler sampler)
{
	VkDescriptorImageInfo	imageInfo{};
	imageInfo.imageView = view;
	imageInfo.sampler = sampler;
	imageInfo.imageLayout = layout;
	_imagesInfo.push_back(imageInfo);

	VkWriteDescriptorSet	write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = _handle->sets[setIndex];
	write.dstBinding = binding;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pImageInfo = &_imagesInfo.back();
	_writes.push_back(write);
	return (*this);
}

void	DescriptorWriter::update(void) {
	vkUpdateDescriptorSets(_device.getLogical(), static_cast<uint32_t>(_writes.size()),
							_writes.data(), 0, nullptr);
}

}
