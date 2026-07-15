/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Frame.cpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 15:47:35 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/15 16:05:39                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Frame.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Descriptors.hpp"

#include <iostream>

namespace	hel {

VkDescriptorSetLayout	Frame::_globalLayout = VK_NULL_HANDLE;

#define RETURN_ERROR(error) do {	\
	_error = error;					\
	return this;					\
} while (0)

GlobalData	*GlobalData::addData(const std::string &key,
								std::shared_ptr<void> data)
{
	if (_error)
		return this;
	if (_locked)
		RETURN_ERROR("Trying to call addData for key \"" + key
					+ "\" when data have already been passed to the engine.");
	if (_engineGlobals.contains(key) || _shaderGlobals.contains(key))
		RETURN_ERROR("Trying to set two data with the key \""
					+ key + "\".");
	if (data == nullptr)
		RETURN_ERROR("Trying to set data as nullptr for \""
					+ key + "\"");
	_engineGlobals.emplace(key, data);
	return this;
}

GlobalData	*GlobalData::addData(const std::string &key,
								std::shared_ptr<void> data,
								Ref<Buffer> buffer, uint32_t bindingIndex)
{
	if (_error)
		return this;
	if (_locked)
		RETURN_ERROR("Trying to call addData for key \"" + key
					+ "\" when data have already been passed to the engine.");
	if (_engineGlobals.contains(key) || _shaderGlobals.contains(key))
		RETURN_ERROR("Trying to set two data with the key \""
					+ key + "\".");
	if (data == nullptr)
		RETURN_ERROR("Trying to set data as nullptr for \""
					+ key + "\"");
	if (buffer == nullptr)
		RETURN_ERROR("Trying to set buffer as nullptr for \""
					+ key + "\"");
	if (std::find_if(_shaderGlobals.begin(), _shaderGlobals.end(),
				[bindingIndex](const std::pair<std::string, ShaderData> &pair) {
					return pair.second.bindingIndex == bindingIndex;
				}) != _shaderGlobals.end())
	{
		RETURN_ERROR("Trying to set the binding "
					+ std::to_string(bindingIndex) + " two times.");
	}
	_shaderGlobals.emplace(key, ShaderData{data, buffer, bindingIndex});
	return this;
}

std::unordered_map<std::string,
				GlobalData::ShaderData>	&GlobalData::list(FrameKey) {
	return _shaderGlobals;
}

expected<void>	GlobalData::lock(EngineKey) {
	if (_locked)
		return unexpected("Error: setUserData's engine function called twice.");
	if (_error.has_value())
		return unexpected(_error.value());
	_locked = true;
	return {};
}


GlobalSetBindings	&GlobalSetBindings::addBinding(uint32_t bindingIndex,
												VkDescriptorType descriptorType,
												VkShaderStageFlags stage)
{
	if (_error.has_value())
		return *this;
	if (_bindings.contains(bindingIndex)) {
		_error = "Trying to add two descriptors on the same binding ("
					+ std::to_string(bindingIndex) + ")";
		return *this;
	}
	auto	newBinding = _bindings.emplace(bindingIndex, Binding{
										.index = bindingIndex,
										.type = descriptorType,
										.stage = stage});
	auto	&bind = newBinding.first->second;
	bind.dynamicBinding =
			(bind.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
			|| bind.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
	return *this;
}

bool	GlobalSetBindings::contains(uint32_t key) {
	return _bindings.contains(key);
}

GlobalSetBindings::Binding	&GlobalSetBindings::operator[](uint32_t index) {
	return _bindings[index];
}



Frame::~Frame(void) {
	if (_commandPool != VK_NULL_HANDLE)
		vkDestroyCommandPool(_device->getLogical(), _commandPool, nullptr);
}

expected<void>	Frame::init(Device *device,
										const GlobalSetBindings &setConfig) {
	if (setConfig._error.has_value())
		return unexpected(setConfig._error.value());
	_device = device;
	_bindingConfig = setConfig;

	return createCommandBuffers()
			.and_then([this]{return createGlobalSets();});
}

expected<void>	Frame::createCommandBuffers() {
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

expected<void>	Frame::createGlobalSets(void) {
	auto	poolFactory = DescriptorPool::Builder(*_device)
							.setPageSize(Swapchain::MAX_FRAMES_IN_FLIGHT);
	for (auto &[index, bind]: _bindingConfig._bindings)
		poolFactory.addDescriptor(bind.type);
	_descriptorPool = poolFactory.build();
	if (!_descriptorPool)
		return unexpected("Couldn't create the global descriptor pool.");

	auto	setFactory = DescriptorFactory(*_device)
					.setSetCount(Swapchain::MAX_FRAMES_IN_FLIGHT);
	for (auto &[index, bind]: _bindingConfig._bindings)
		setFactory.addBinding(index, bind.type, bind.stage);
	_descriptorSets = setFactory.build(*_descriptorPool.get());
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

expected<void>	Frame::bindBuffers(GlobalData *globalData) {
	_setStride = 0;
	for (auto &[key, data]: globalData->list({})) {
		if (!_bindingConfig.contains(data.bindingIndex))
			return unexpected("Trying to bind on an undefined binding index ("
							+ std::to_string(data.bindingIndex) + ")");
		auto	&bind = _bindingConfig[data.bindingIndex];
		if (bind.buffer != nullptr)
			return unexpected("The binding index "
						+ std::to_string(bind.index)
						+ " already have buffer attached.");
		auto	buffer = data.buffer;
		if (!buffer)
			return unexpected("Trying to bind a nullptr buffer on index "
						+ std::to_string(data.bindingIndex) + ".");
		uint32_t	requiredCount = Swapchain::MAX_FRAMES_IN_FLIGHT
								* (bind.dynamicBinding ? MAX_PASS_COUNT : 1);
		if (buffer->getSize() < buffer->getStride() * requiredCount)
			return tl::unexpected("Trying to bind on index "
					+ std::to_string(data.bindingIndex)
					+ " a buffer too small for it's usage. Expected a count of "
					+ std::to_string(requiredCount) + ".");

		bind.buffer = buffer;
		_setStride += buffer->getStride();
		for (uint32_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; i++) {
			_writer->writeBuffer(i, data.bindingIndex, buffer, i * MAX_PASS_COUNT);
		}
	}
	_writer->update();
	return {};
}

expected<void>	Frame::validateGlobalSet(void) {
	for (auto &[index, bind]: _bindingConfig._bindings) {
		if (bind.buffer == nullptr)
			return unexpected("The binding " + std::to_string(index)
								+ " doesn't have a buffer.");
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
	for (auto &[key, write]: ctx.globals->list({}))
		writeToUBO(write.data.get(), write.bindingIndex,
					ctx.passIndex, ctx.frameIndex);
}

void	Frame::writeToUBO(void *data, uint32_t bindingIndex,
						uint32_t passIndex, uint32_t frameIndex) {
	auto		&bind = _bindingConfig[bindingIndex];
	auto		stride = bind.buffer->getStride();
	uint32_t	offset = bind.dynamicBinding
							? (frameIndex * Frame::MAX_PASS_COUNT + passIndex)
							: (frameIndex);
	offset *= stride;
	if (auto res = bind.buffer->writeToBuffer(data, stride, offset); !res) {
		std::cerr << "Error on write on UBO: " << res.error() << std::endl; 
	}
}

}
