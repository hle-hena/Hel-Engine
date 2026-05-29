/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/05/29 17:00:16                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Engine.hpp"
#include "core/Queues.hpp"

#include "api/vulkan/Device.hpp"
#include "api/vulkan/Swapchain.hpp"
#include "api/vulkan/Sampler.hpp"
#include "api/vulkan/Descriptors.hpp"
#include "api/vulkan/Renderer.hpp"

#include "utils/healthHelper.hpp"

#include "platform/window/Window.hpp"
#include "platform/window/GLFW.hpp"
#include "platform/ui/UI.hpp"

#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"

#include <iostream>

#include "utils/VFS.hpp"

namespace hel {

Engine::Engine(Device &device, Registry &registry)
	:	_device{device},
		_registry{registry} {
	_timer.start();
	VFS::load({});
}

Engine::~Engine(void) {
	_systems.clear({});
	Sampler::deleteAllSamplers(_device);
	DescriptorFactory::deleteLayoutCache(_device);
	if (_commandPool != VK_NULL_HANDLE)
		vkDestroyCommandPool(_device.getLogical(), _commandPool, nullptr);
}

bool	Engine::init(Window &window) {
	if (createCommandPool())
		return (true);
	createDescriptorPools();
	auto	frameRes = _frame.init(_device, _staticPool.get(), _commandPool);
	if (!frameRes) {
		std::cerr << frameRes.error() << std::endl;
		return (true);
	}
	createImagePool();

	auto	frameCtx = _frame.getContext(&window, 0, 0);
	_engineCtx.device = &_device;
	_engineCtx.imagePool = _imagePool.get();
	_engineCtx.registry = &_registry;

	for (auto &system: _systems.getSystems()) {
		system->init(_engineCtx, frameCtx);
		system->init();
	}
	_systems.sort({});
	return (false);
}

bool	Engine::createCommandPool(void) {
	VkCommandPoolCreateInfo	commandPoolInfo{};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = _device.getQueueFamily().graphicsFamily.value();

	if (vkCreateCommandPool(_device.getLogical(), &commandPoolInfo, nullptr, &_commandPool) != VK_SUCCESS)
		RETURN_SET_UNHEALTHY("Couldn't create the command pool.", true);
	return (false);
}

void	Engine::createDescriptorPools(void) {
	_staticPool = DescriptorPool::Builder(_device)
		.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
		.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f)
		.setPageSize(GLFW::_maxInstanceCount * Swapchain::MAX_FRAMES_IN_FLIGHT)
		.build();
}

void	Engine::createImagePool(void) {
	_imagePool = ImagePool::Builder(_device)
		.addImage(Image::Config()
			.setHeight(4096)
			.setWidth(4096)
			.setFormats({VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM})
			.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT)
			.setUsage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			.setAspect(VK_IMAGE_ASPECT_COLOR_BIT))
		.addImage(Image::Config()
			.setHeight(4096)
			.setWidth(4096)
			.setFormats(VK_FORMAT_D32_SFLOAT_S8_UINT)
			.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
		.build();
}

void	Engine::tick(Window *window, uint32_t frameIndex) {
	window->getSwapchain().waitForFrameFence(frameIndex);
	auto	frameCtx = _frame.getContext(window, frameIndex, _lastFrameTime);
	auto	&ui = window->getUI();
	_lastFrameTime = _timer.lap();
	_imagePool->releaseAll();

	updateTick(ui, frameCtx);
	_registry.updateBuffers(_device);
	renderTick(window, ui, frameCtx);
}

void	Engine::updateTick(UiContext &ui, FrameContext &frameCtx) {
	ui.newFrame();
	for (auto &system: _systems.getUpdateInteractions())
		system->updateInteraction(frameCtx);
	ui.endFrame();
	for (auto &system: _systems.getUpdates())
		system->update(frameCtx);
}

void	Engine::renderTick(Window *window, UiContext &ui, FrameContext &ctx) {
	Swapchain	&swapchain = window->getSwapchain();
	uint32_t	imageIndex;

	RenderPass::newFrame();
	if (swapchain.acquireNextImage(*window, ctx.frameIndex, &imageIndex))
		return ;
	vkResetCommandBuffer(ctx.commandBuffer, 0);
	ctx.descriptorPool->resetPools();

	auto	depthImage = _imagePool->acquire(Image::Config()
			.setFormats(VK_FORMAT_D32_SFLOAT_S8_UINT)
			.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
	auto	swapImage = swapchain.getSwapImage(imageIndex);

	VkCommandBufferBeginInfo	beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(ctx.commandBuffer, &beginInfo))
		return ;
	for (auto &renderRequest: RenderQueue::flush()) {
		Image	*entityImg = _imagePool->acquire(Image::Config()
							.setFormats({VK_FORMAT_R32_UINT})
							.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
							.setUsage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
							.setAspect(VK_IMAGE_ASPECT_COLOR_BIT));
		auto	renderImg = renderRequest.mainImage;
		ctx.request = &renderRequest;
		renderRequest.secondaryImages["entityID"] = entityImg;
		updateGlobalData(ctx);
		VkClearValue	clear{};
		clear.color.uint32[0] = 0xFFFFFFFF;
		if (auto renderer = RenderPass(_device, ctx.commandBuffer, renderImg->getExtent())
						.setDepthStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
						.addColorWrite(renderImg, VK_FORMAT_B8G8R8A8_SRGB)
						.setClearValue(clear)
						.addColorWrite(entityImg, VK_FORMAT_R32_UINT)
						.addDepthWrite(depthImage, depthImage->getFormat())
						.beginPass(ctx)) {
			writeGlobalData(renderer);
			for (auto &system: _systems.getRenders())
				system->render(renderer);
		}
		if (auto renderer = RenderPass(_device, ctx.commandBuffer, renderImg->getExtent())
						.setColorLoadOp(VK_ATTACHMENT_LOAD_OP_LOAD)
						.setDepthLoadOp(VK_ATTACHMENT_LOAD_OP_LOAD)
						.setDepthStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
						.addColorWrite(renderImg, VK_FORMAT_B8G8R8A8_SRGB)
						.addDepthWrite(depthImage, depthImage->getFormat())
						.beginPass(ctx)) {
			writeGlobalData(renderer);
			for (auto &system: _systems.getPostProcess())
				system->postProcessing(renderer);
		}
		if (auto renderer = RenderPass(_device, ctx.commandBuffer, renderImg->getExtent())
						.setColorLoadOp(VK_ATTACHMENT_LOAD_OP_LOAD)
						.addColorWrite(renderImg, VK_FORMAT_B8G8R8A8_SRGB)
						.addColorWrite(entityImg, VK_FORMAT_R32_UINT)
						.addDepthWrite(depthImage, depthImage->getFormat())
						.beginPass(ctx)) {
			writeGlobalData(renderer);
			for (auto &system: _systems.getRenderInteractions())
				system->renderInteraction(renderer);
		}
		if (auto renderer = RenderPass(_device, ctx.commandBuffer, renderImg->getExtent())
						.setColorLoadOp(VK_ATTACHMENT_LOAD_OP_LOAD)
						.addColorWrite(renderImg, VK_FORMAT_B8G8R8A8_SRGB)
						.addColorWrite(entityImg, VK_FORMAT_R32_UINT)
						.addDepthWrite(depthImage, depthImage->getFormat())
						.beginPass(ctx)) {
			writeGlobalData(renderer);
			DrawQueue::execute();
		}
		renderImg->transitionLayout(ctx.commandBuffer,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	if (auto renderer = RenderPass(_device, ctx.commandBuffer, swapImage->getExtent())
					.addColorWrite(swapImage, VK_FORMAT_B8G8R8A8_UNORM)
					.beginPass(ctx)) {
		ui.renderFrame(ctx.commandBuffer);
	}
	swapImage->transitionLayout(ctx.commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	Read::Queue::execute(ctx.commandBuffer);

	vkEndCommandBuffer(ctx.commandBuffer);

	swapchain.submitCommandBuffer(ctx.commandBuffer, imageIndex, ctx.frameIndex);
	swapchain.present(*window, imageIndex);
}

void	Engine::updateGlobalData(FrameContext &ctx) {
	auto	handle = ctx.request->handle;
	ctx.globalData.viewProjection = glm::mat4{1.f};
	if (auto camera = _registry.getComponent<comp::Camera>(handle)) {
		auto	extent = ctx.request->mainImage->getExtent();
		float	aspect = static_cast<float>(extent.width) /
						static_cast<float>(extent.height);
		glm::mat4 projection = glm::perspective(glm::radians(camera->fov), aspect, camera->near, camera->far);
		projection[1][1] *= -1;
		ctx.projection = projection;
		ctx.globalData.viewProjection = projection * camera->view;
	}
	ctx.globalData.elapsedTime = _timer.elapsedTime();
}

void	Engine::writeGlobalData(Renderer &renderer) {
	auto	ctx = renderer.frameContext();
	_frame.writeToUBO(&ctx.globalData, renderer.passIndex(), ctx.frameIndex);
}

}
