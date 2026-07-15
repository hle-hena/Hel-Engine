/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pool.tpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 16:53:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/15 15:54:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/Pool.hpp"
#include "ecs/IComponent.hpp"
#include "api/vulkan/Swapchain.hpp"

namespace	hel {

template <ValidComponent Component>
bool	Pool<Component>::has(Entity::id handle) const {
	uint32_t	idx = Entity::getIndex(handle);
	return (idx < indices.size() && indices[idx] != Entity::NOT_REGISTERED);
}

template <ValidComponent Component>
OpaqueComponentHandle	Pool<Component>::get(Entity::id handle) {
	OpaqueComponentHandle	comp;
	comp._pool = this;
	comp._index = indices[Entity::getIndex(handle)];
	return comp;
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
expected<void>	Pool<Component>::syncBuffer() {
	if constexpr (!Component::MetaData::gpuVisible) {
		return {};
	} else
	{
	using GPULayout = typename Component::GPULayout;

	uint32_t	nComp = static_cast<uint32_t>(components.size());
	std::vector<GPULayout>	gpuData;
	gpuData.reserve(nComp);
	for (auto &comp: components)
		gpuData.push_back(Component::MetaData::toGPU(comp));
	auto	res = buffer->writeToBuffer(gpuData.data(), nComp);
	if (!res)
		return unexpected(res.error());
	if (*res)
		_pendingBuffers.push_back({Swapchain::MAX_FRAMES_IN_FLIGHT, *res});
	return {};
	}
}

template <ValidComponent Component>
expected<void>	Pool<Component>::syncBuffer(const PendingWrite &write) {
	if constexpr (!Component::MetaData::gpuVisible) {
		return {};
	} else
	{
	auto	&comp = *static_cast<Component::POD*>(write.data);
	auto	gpuData = Component::MetaData::toGPU(comp);
	auto	res = buffer->writeToBuffer(&gpuData, 1, write.index);
	if (!res)
		return unexpected(res.error());
	if (*res)
		_pendingBuffers.push_back({Swapchain::MAX_FRAMES_IN_FLIGHT, *res});
	return {};
	}

}

template <ValidComponent Component>
expected<void>	Pool<Component>::flushWrites(Device &device) {
	expected<void>	res;
	if constexpr (Component::MetaData::gpuVisible) {
		if (!buffer) {
			auto	res = Buffer::create<typename Component::GPULayout>(&device,
				BufferConfig()
				.usage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
				.allocFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
					VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT));
			if (!res)
				return unexpected(res.error());
			buffer = *res;
		}

		if (GPUBufferDirty) {
			auto	r = syncBuffer();
			if (!r && res) res = r;
		}
		else {
			for (auto &write: _writes) {
				auto	r = syncBuffer(write);
				if (!r && res) res = r;
			}
		}
	}
	GPUBufferDirty = false;
	_writes.clear();
	return res;
}

template <ValidComponent Component>
void	Pool<Component>::removePendingBuffers(void) {
	std::erase_if(_pendingBuffers, [](auto &item){
		auto	&[frameLeft, buffer] = item;
		return (frameLeft-- == 0);
	});
}

}
