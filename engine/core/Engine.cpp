/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/24 17:39:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/25 17:24:39                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/Engine.hpp"
#include "core/scheduler/RenderQueue.hpp"
#include "core/scheduler/ReadQueue.hpp"
#include "core/scheduler/DrawQueue.hpp"
#include "core/scheduler/CycleEntry.hpp"
#include "core/ecs/ISystem.hpp"

#include "rhi/window/Window.hpp"
#include "rhi/window/UiContext.hpp"
#include "rhi/resources/ImagePool.hpp"
#include "rhi/render/Renderer.hpp"
#include "rhi/window/GLFW.hpp"
#include "rhi/resources/Sampler.hpp"

namespace	hel {

Engine::Engine(void)
{
}

Engine::~Engine(void) {
	_userData = nullptr;
	_systems.clear({});
	if (_device) {
		Sampler::deleteAllSamplers(*_device);
		DescriptorFactory::deleteLayoutCache(*_device);
	}
	GLFW::release();
}

expected<void>	Engine::init(const EngineConfig &config)
{
	_config = config;
	if (!GLFW::acquire())
		return tl::unexpected("Couldn't init glfw.");
	auto	vkInit = _vkContext.initiateVulkan();
	if (!vkInit)
		return tl::unexpected(vkInit.error());
	_device = _vkContext.getDevice();
	auto	res = createWindow(Window::WIDTH, Window::HEIGHT, "Hel-Engine")
			.and_then([this]{
				return _frame.init(_device, _config.defineGlobalSet());
			}).and_then([this]{ return createImagePool(); });
	if (!res)
		return tl::unexpected(res.error());

	_registry.init(_device);
	for (auto &system: _systems.getSystems())
		system->init(_device, &_registry, _imagePool.get(), &_inputState);
	_systems.sort({});

	_config.loadPrimaryScene(&_registry, _appWindow.get());
	return {};
}

expected<void>	Engine::createWindow(int width, int height,
							const std::string &windowName)
{
	Window::windowPtr window = Window::createWindow(static_cast<uint32_t>(width),
								static_cast<uint32_t>(height), windowName,
								&_vkContext);
	if (!window)
		return tl::unexpected("Failed to create the window.");
	if (!_vkContext.getDevice()->supportSurface(window.get()))
		return tl::unexpected("The window surface is not supported.");
	_appWindow = std::move(window);
	return {};
}

expected<void>	Engine::createImagePool(void) {
	_imagePool = ImagePool::create(_device);
	return {};
}

expected<void>	Engine::setUserData(std::shared_ptr<GlobalData> data) {
	_userData = data;
	return _userData->lock({})
			.and_then([this]{ return _frame.bindBuffers(_userData.get()); })
			.and_then([this]{ return _frame.validateGlobalSet(); });
}



void	Engine::run(void) {
	uint32_t	currentFrame = 0;

	while (_appWindow) {
		_inputState.newFrame(_appWindow->pollEvents());

		if (_appWindow->shouldClose())
			break ;

		tick(currentFrame);

		currentFrame = (currentFrame + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
		_registry.resetAllDirty();
	}
	vkDeviceWaitIdle(_vkContext.getDevice()->getLogical());
}

void	Engine::tick(uint32_t frameIndex) {
	_appWindow->getSwapchain().waitForFrameFence(frameIndex);

	ExecutionContext	ctx(frameIndex, _userData.get());
	_frame.fillContext(ctx, _appWindow.get());
	if (_appWindow->getSwapchain().acquireNextImage(*_appWindow.get(), frameIndex, &ctx.swapIndex))
		return ;
	_imagePool->collectFromFrame(frameIndex);
	_imagePool->evict();
	_config.tickCallback(&_registry, ctx);

	updateTick(ctx);
	_registry.updateBuffers(*_device);
	renderTick(_appWindow.get(), ctx);
}

void	Engine::updateTick(ExecutionContext &execCtx) {
	UiContext::newFrame();
	for (auto &func: _systems.getUpdates())
		func->execute(execCtx);
	UiContext::endFrame();
}

void	Engine::renderTick(Window *window, ExecutionContext &execCtx) {
	Swapchain	&swapchain = window->getSwapchain();

	RenderPass::newFrame();

	auto	swapImage = swapchain.getSwapImage(execCtx.swapIndex);
	vkResetCommandBuffer(execCtx.commandBuffer, 0);
	execCtx.descriptorPool->resetPools();

	VkCommandBufferBeginInfo	beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(execCtx.commandBuffer, &beginInfo))
		return ;

	for (auto &renderRequest: RenderQueue::flush()) {
		execCtx.request = &renderRequest;
		_config.updateGlobalData(&_registry, execCtx);

		for (auto &funcList: _systems.getRenders(renderRequest.requestType))
			executePass(execCtx, funcList);

		for (auto &level: DrawQueue::flush()) {
			for (auto &pass: level.second) {
				if (auto renderer = RenderPass(*_device, execCtx, _imagePool.get(),
										pass.dep)
									.beginPass()) {
					_frame.writeGlobalData(renderer.executionContext());
					for (auto &drawCommand: pass.draws)
						drawCommand.submit();
				}
			}
		}
	}

	swapImage->transitionLayout(execCtx.commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	Read::Queue::execute(execCtx.commandBuffer);

	vkEndCommandBuffer(execCtx.commandBuffer);

	swapchain.submitCommandBuffer(execCtx.commandBuffer, execCtx.swapIndex, execCtx.frameIndex);
	swapchain.present(*window, execCtx.swapIndex);
}

void	Engine::executePass(ExecutionContext &ctx, const SystemManager::EntryVec &funcs) {
	if (funcs.empty())
		return ;
	if (auto renderer = RenderPass(*_device, ctx, _imagePool.get(), funcs)
						.beginPass()) {
		_frame.writeGlobalData(renderer.executionContext());
		for (auto &func: funcs)
			func->execute(renderer);
	}
}

}
