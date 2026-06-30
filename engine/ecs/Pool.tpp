/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pool.tpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 16:53:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/30 16:57:48                                        */
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
void	Pool<Component>::syncBuffer(Device &device) {
	if constexpr (requires { Component::gpuVisible == true; }) {
		uint32_t	nbComp = static_cast<uint32_t>(components.size());
		using BufferType = std::conditional_t<
			requires { typename Component::GPUType; },
			typename Component::GPUType,
			Component>;
		if (!buffer || buffer->getSize() < nbComp * sizeof(BufferType)) {
			if (buffer)
				_pendingBuffers.push_back({Swapchain::MAX_FRAMES_IN_FLIGHT, std::move(buffer)});
			buffer = Buffer::create(device, sizeof(BufferType) * std::max(nbComp, 8u),
						1,
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
						VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
						VMA_ALLOCATION_CREATE_MAPPED_BIT);
		}
		if constexpr (requires (Component c) { c.toGPU(); }) {
			std::vector<BufferType>	gpuData;
			gpuData.reserve(nbComp);
			for (auto &comp: components)
				gpuData.push_back(comp.toGPU());
			buffer->writeToBuffer(gpuData.data(), nbComp * sizeof(BufferType));
		} else {
			buffer->writeToBuffer(components.data(), nbComp * sizeof(Component));
		}
	}
	isDirty = false;
}

template <ValidComponent Component>
void	Pool<Component>::syncBuffer(Device &, const PendingWrite &write) {
	if constexpr (requires { Component::gpuVisible == true; }) {
		if constexpr (requires (Component c) { c.toGPU(); }) {
			auto		gpuData = static_cast<Component *>(write.data)->toGPU();
			uint32_t	gpuOffset = write.index * sizeof(typename Component::GPUType);
			buffer->writeToBuffer(&gpuData, sizeof(typename Component::GPUType), gpuOffset);
		} else {
			buffer->writeToBuffer(write.data, sizeof(Component), write.index * sizeof(Component));
		}
	}
}

template <ValidComponent Component>
void	Pool<Component>::removeEntity(Entity::id handle) {
	uint32_t		entityIndex = Entity::getIndex(handle);
	if (entityIndex >= indices.size() || indices[entityIndex] == Entity::NOT_REGISTERED)
		return;
	uint32_t		lastIndex = static_cast<uint32_t>(components.size()) - 1;
	uint32_t		removedIndex = indices[entityIndex];
	if (removedIndex != lastIndex) {
		components[removedIndex] = std::move(components[lastIndex]);
		indices[Entity::getIndex(entities[lastIndex])] = removedIndex;
		entities[removedIndex] = entities[lastIndex];
	}
	components.resize(lastIndex);
	entities.resize(lastIndex);
	indices[entityIndex] = Entity::NOT_REGISTERED;
	isDirty = true;
}

template <ValidComponent Component>
void	Pool<Component>::resetDirtyFlag(void) {
	if constexpr (requires(Component c) { c.isDirty = false; }) {
		for (auto &comp : components) {
			comp.isDirty = false;
		}
	}
}

template <ValidComponent Component>
void	Pool<Component>::addWrite(uint32_t index, void *data) {
	PendingWrite	write{index, data};
	auto	[it, inserted] = _writes.insert(write);

	if (!inserted) {
		_writes.erase(it);
		_writes.insert(write);
	}
}

template <ValidComponent Component>
void	Pool<Component>::flushWrites(Device &device) {
	if (isDirty) {
		_writes.clear();
		return (syncBuffer(device));
	}
	for (auto &write: _writes) {
		syncBuffer(device, write);
	}
	_writes.clear();
}

template <ValidComponent Component>
void	Pool<Component>::removePendingBuffers(void) {
	std::erase_if(_pendingBuffers, [](auto &item){
		auto	&[frameLeft, buffer] = item;
		return (frameLeft-- == 0);
	});
}

template <ValidComponent Component>
bool	Pool<Component>::has(Entity::id handle) const {
	uint32_t	idx = Entity::getIndex(handle);
	return (idx < indices.size() && indices[idx] != Entity::NOT_REGISTERED);
}

template <ValidComponent Component>
void	*Pool<Component>::getRaw(Entity::id handle) {
	return (&components[indices[Entity::getIndex(handle)]]);
}

template <ValidComponent Component>
const char	*Pool<Component>::getTypeName(void) const {
	return Component::MetaData::label;
}

}
