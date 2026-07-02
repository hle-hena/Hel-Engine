/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pool.tpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 16:53:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/02 14:27:07                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/Pool.hpp"
#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Swapchain.hpp"
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace	hel {

template <ValidComponent Component>
bool	Pool<Component>::has(Entity::id handle) const {
	uint32_t	idx = Entity::getIndex(handle);
	return (idx < indices.size() && indices[idx] != Entity::NOT_REGISTERED);
}

template <ValidComponent Component>
OpaqueComponentHandle	Pool<Component>::get(Entity::id handle) {
	return {._pool = this, ._index = indices[Entity::getIndex(handle)]};
}

template <ValidComponent Component>
void	*Pool<Component>::getRaw(uint32_t index) {
	return (&components[index]);
}

template <ValidComponent Component>
std::string_view	Pool<Component>::getTypeName(void) const {
	return Component::MetaData::label;
}

template <ValidComponent Component>
void	Pool<Component>::removeEntity(Entity::id handle) {
	uint32_t	entityIndex = Entity::getIndex(handle);
	if (entityIndex >= indices.size() || indices[entityIndex] == UNDEFINED)
		return ;
	uint32_t	lastIndex = static_cast<uint32_t>(components.size()) - 1;
	uint32_t	removedIndex = indices[entityIndex];
	if (removedIndex != lastIndex) {
		components[removedIndex] = std::move(components[lastIndex]);
		compDirty[removedIndex] = compDirty[lastIndex];
		indices[Entity::getIndex(entities[lastIndex])] = removedIndex;
		entities[removedIndex] = entities[lastIndex];
	}
	components.resize(lastIndex);
	entities.resize(lastIndex);
	compDirty.resize(lastIndex);
	indices[entityIndex] = UNDEFINED;
	GPUBufferDirty = true;
}



template <ValidComponent Component>
void	Pool<Component>::syncBuffer(Device &dev) {
	if (Component::MetaData::gpuVisible) {
		uint32_t	nbComp = static_cast<uint32_t>(components.size());
		if (!buffer || buffer->getSize() < nbComp * buffer->getStride()) {
			if (buffer)
				_pendingBuffers.push_back({Swapchain::MAX_FRAMES_IN_FLIGHT,
											std::move(buffer)});
			buffer = Buffer::create(dev, sizeof(Component::GPULayout), nbComp,
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
						VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
						| VMA_ALLOCATION_CREATE_MAPPED_BIT);
		}
		std::vector<typename Component::GPULayout>	gpuData;
		gpuData.reserve(nbComp);
		for (auto &comp: components)
			gpuData.push_back(Component::MetaData::toGPU(comp));
		buffer->writeToBuffer(gpuData.data());
	}
	GPUBufferDirty = false;
}

template <ValidComponent Component>
void	Pool<Component>::syncBuffer(Device &, const PendingWrite &write) {
	if (Component::MetaData::gpuVisible) {
			auto	&comp = *static_cast<Component::POD*>(write.data);
			auto	gpuData = Component::MetaData::toGPU(comp);
			uint32_t	stride = buffer->getStride();
			buffer->writeToBuffer(&gpuData, stride, write.index * stride);
	}
}

template <ValidComponent Component>
void	Pool<Component>::flushWrites(Device &device) {
	if (GPUBufferDirty) {
		_writes.clear();
		return syncBuffer(device);
	}
	for (auto &write: _writes)
		syncBuffer(device, write);
	_writes.clear();
}

template <ValidComponent Component>
void	Pool<Component>::removePendingBuffers(void) {
	std::erase_if(_pendingBuffers, [](auto &item){
		auto	&[frameLeft, buffer] = item;
		return (frameLeft-- == 0);
	});
}

}
