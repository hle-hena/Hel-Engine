/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Frame.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 15:47:35 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/27 20:49:45                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Frame.hpp"
#include "api/vulkan/Device.hpp"
#include <iostream>

namespace	hel {

VkDescriptorSetLayout	Frame::_globalLayout = VK_NULL_HANDLE;


GlobalSetBindings	&GlobalSetBindings::addBinding(Buffer *buffer, uint32_t bindingIndex,
												VkDescriptorType descriptorType,
												VkShaderStageFlags stage)
{
	if (!_error.empty())
		return *this;
	if (buffer == nullptr) {
		_error = "The buffer given to bind was a nullptr.";
		return *this;
	}
	if (_bindings.contains(bindingIndex)) {
		_error = "Trying to add two descriptors on the same binding ("
					+ std::to_string(bindingIndex) + ")";
		return *this;
	}
	auto	newBinding = _bindings.emplace(bindingIndex, Binding{
										.index = bindingIndex,
										.type = descriptorType,
										.stage = stage,
										.buffer = buffer});
	auto	&bind = newBinding.first->second;
	bind.dynamicBinding =
			(bind.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
			|| bind.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
	return *this;
}

expected<GlobalSetBindings>	GlobalSetBindings::build(void) {
	if (!_error.empty())
		return unexpected(_error);
	return *this;
}

bool	GlobalSetBindings::contains(uint32_t key) {
	return _bindings.contains(key);
}

GlobalSetBindings::Binding	&GlobalSetBindings::operator[](uint32_t index) {
	return _bindings[index];
}


tl::expected<void, std::string>	Frame::init(Device *device,
										const GlobalSetBindings &setConfig) {
	_device = device;
	_bindingConfig = setConfig;

	return createCommandBuffers()
			.and_then([this]{return createGlobalSets();})
			.and_then([this]{return bindBuffers();});
}

tl::expected<void, std::string>	Frame::createCommandBuffers() {
	VkCommandPoolCreateInfo	commandPoolInfo{};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = _device->getQueueFamily().graphicsFamily.value();

	if (vkCreateCommandPool(_device->getLogical(), &commandPoolInfo,
			nullptr, &_commandPool) != VK_SUCCESS)
		return tl::unexpected("Couldn't create the command pool.");

	VkCommandBufferAllocateInfo	cbAllocInfo{};
	cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbAllocInfo.commandPool = _commandPool;
	cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cbAllocInfo.commandBufferCount = Swapchain::MAX_FRAMES_IN_FLIGHT;
	if (vkAllocateCommandBuffers(_device->getLogical(), &cbAllocInfo,
		_commandBuffers.data()))
		return tl::unexpected("Failed to allocated command buffers");
	return {};
}

tl::expected<void, std::string>	Frame::createGlobalSets(void) {
	_descriptorPool = DescriptorPool::Builder(*_device)
		.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 2)
		.setPageSize(Swapchain::MAX_FRAMES_IN_FLIGHT)
		.build();
	if (!_descriptorPool)
		return unexpected("Couldn't create the global descriptor pool.");

	_descriptorSets = DescriptorFactory(*_device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
		.setSetCount(Swapchain::MAX_FRAMES_IN_FLIGHT)
		.build(*_descriptorPool);
	if (!_descriptorSets)
		return unexpected("Couldn't create the global descriptor sets.");
	_globalLayout = _descriptorSets->setLayout;

	_writer = std::make_unique<DescriptorWriter>(*_device, _descriptorSets.get());
	for (uint32_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; i++) {
		//TODO -> Allow the user to decide what is in the dynamic pool.
		_dynamicPools[i] = DescriptorPool::Builder(*_device)
			.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1.f)
			.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f)
			.addDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10.f)
			.setPageSize(1000)
			.build();
	}
	return {};
}

tl::expected<void, std::string>	Frame::bindBuffers(void) {
	for (auto &[index, bind]: _bindingConfig._bindings) {
		auto	*buffer = bind.buffer;
		uint32_t	requiredCount = Swapchain::MAX_FRAMES_IN_FLIGHT
									* (bind.dynamicBinding ? MAX_PASS_COUNT : 1);
		if (buffer->getSize() < buffer->getStride() * requiredCount)
			return tl::unexpected("Trying to bind on index " + std::to_string(index)
						+ " a buffer too small for it's usage. Expected a count of "
						+ std::to_string(requiredCount) + ".");

		_setStride += buffer->getStride();
		for (uint32_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; i++) {
			_writer->writeBuffer(i, index, bind.type, *buffer,
								i * buffer->getStride() * MAX_PASS_COUNT);
		}
		_writer->update();
	}

	return {};
}

void	Frame::fillContext(FrameContext &context, Window *window) {
	auto	frameIndex = context.frameIndex;
	context.window = window;
	context.commandBuffer = _commandBuffers[frameIndex];
	context.globalSet = _descriptorSets->sets[frameIndex];
	context.setStride = _setStride;
	context.globalLayout = _globalLayout;
	context.descriptorPool = _dynamicPools[frameIndex].get();
	context.frameIndex = frameIndex;
}

void	Frame::writeGlobalData(FrameContext &ctx) {
	for (auto write: *ctx.globalData)
		writeToUBO(write.data, write.bindingIndex,
				ctx.passIndex, ctx.frameIndex);
}

void	Frame::writeToUBO(void *data, uint32_t bindingIndex,
						uint32_t passIndex, uint32_t frameIndex) {
	auto		&bind = _bindingConfig[bindingIndex];
	if (!bind.buffer) {
		std::cout << "Writing to a null buffer on bindingIndex "
				<< bindingIndex << std::endl;
		return ;
	}
	auto		stride = bind.buffer->getStride();
	uint32_t	offset = bind.dynamicBinding
							? (frameIndex * Frame::MAX_PASS_COUNT + passIndex)
							: (frameIndex);
	offset *= stride;
	bind.buffer->writeToBuffer(data, stride, offset);
}

}
