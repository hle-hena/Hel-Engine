/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/20 18:55:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/12 14:39:17                                        */
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
#include <ranges>

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
	bool	shouldDoRenderTick = true;
	if (window->getSwapchain().acquireNextImage(*window, frameCtx.frameIndex,
												&frameCtx.swapIndex))
		shouldDoRenderTick = false;

	updateTick(ui, frameCtx);
	_registry.updateBuffers(_device);
	if (shouldDoRenderTick)
		renderTick(window, ui, frameCtx);
}

void	Engine::updateTick(UiContext &ui, FrameContext &frameCtx) {
	ui.newFrame();
	for (auto &func: _systems.getUpdates())
		func->execute(frameCtx);
	ui.endFrame();
}

void	Engine::renderTick(Window *window, UiContext &, FrameContext &ctx) {
	Swapchain	&swapchain = window->getSwapchain();

	RenderPass::newFrame();

	auto	swapImage = swapchain.getSwapImage(ctx.swapIndex);
	vkResetCommandBuffer(ctx.commandBuffer, 0);
	ctx.descriptorPool->resetPools();

	VkCommandBufferBeginInfo	beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(ctx.commandBuffer, &beginInfo))
		return ;

	for (auto &renderRequest: RenderQueue::flush()) {
		auto matchingType = [&renderRequest](const sys::ISystem *sys) {
			auto	types = sys->getRenderTypes();
			return (std::ranges::find(types, renderRequest.requestType)
					!= types.end());
		};

		ctx.request = &renderRequest;
		updateGlobalData(ctx);

		for (auto &renderSystems: _systems.getRenders())
			executePass(ctx, renderSystems | std::views::filter(matchingType),
						&sys::ISystem::renderDeps, &sys::ISystem::render);
		for (auto &postSystems: _systems.getPostProcess())
			executePass(ctx, postSystems | std::views::filter(matchingType),
						&sys::ISystem::postProcessDeps,
						&sys::ISystem::postProcessing);
		executePass(ctx,
			_systems.getRenderInteractions() | std::views::filter(matchingType),
			&sys::ISystem::renderInterDeps, &sys::ISystem::renderInteraction);
		for (auto &level: DrawQueue::flush()) {
			for (auto &pass: level.second) {
				if (auto renderer = RenderPass(_device, ctx, _imagePool.get(),
										pass.dep)
									.beginPass()) {
					writeGlobalData(renderer);
					for (auto &drawCommand: pass.draws)
						drawCommand.submit();
				}
			}
		}
	}

	swapImage->transitionLayout(ctx.commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	Read::Queue::execute(ctx.commandBuffer);

	vkEndCommandBuffer(ctx.commandBuffer);

	swapchain.submitCommandBuffer(ctx.commandBuffer, ctx.swapIndex, ctx.frameIndex);
	swapchain.present(*window, ctx.swapIndex);
}

void	Engine::executePass(FrameContext &ctx,
						auto &&systems,
						PhaseDependencies sys::ISystem::*depMember,
						void (sys::ISystem::*method)(const Renderer&)) {
	if (systems.empty())
		return ;
	if (auto renderer = RenderPass(_device, ctx, _imagePool.get(),
							systems, depMember)
						.beginPass()) {
		writeGlobalData(renderer);
		for (auto &system: systems)
			(system->*method)(renderer);
	}
}

void	Engine::updateGlobalData(FrameContext &ctx) {
	auto	handle = ctx.request->handle;
	ctx.globalData.viewProjection = glm::mat4{1.f};
	if (auto camera = _registry.getComponent<comp::Camera>(handle)) {
		auto	extent = ctx.request->images["mainColor"]->getExtent();
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
