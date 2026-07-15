/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Engine.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/24 17:39:01 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/15 17:37:45                                        */
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
#include "platform/window/Window.hpp"
#include "platform/ui/UiContext.hpp"
#include "api/vulkan/ImagePool.hpp"
#include "core/RenderQueue.hpp"
#include "core/ReadQueue.hpp"
#include "core/DrawQueue.hpp"
#include "api/vulkan/Renderer.hpp"
#include "platform/window/GLFW.hpp"
#include "api/vulkan/Sampler.hpp"
#include "ecs/CycleEntry.hpp"
#include "ecs/ISystem.hpp"

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
								&_vkContext, &_inputState);
	if (!window)
		return tl::unexpected("Failed to create the window.");
	if (!_vkContext.getDevice()->supportSurface(window.get()))
		return tl::unexpected("The window surface is not supported.");
	_appWindow = std::move(window);
	return {};
}

expected<void>	Engine::createImagePool(void) {
	_imagePool = ImagePool::Builder(*_vkContext.getDevice())
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
		_inputState.newFrame();
		_appWindow->pollEvents();

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

	FrameContext	ctx(frameIndex, _userData.get());
	_frame.fillContext(ctx, _appWindow.get());
	_imagePool->releaseAll();
	bool	shouldDoRenderTick = true;
	if (_appWindow->getSwapchain().acquireNextImage(*_appWindow.get(), frameIndex, &ctx.swapIndex))
		shouldDoRenderTick = false;
	_config.tickCallback(&_registry, ctx);

	updateTick(ctx);
	_registry.updateBuffers(*_device);
	if (shouldDoRenderTick)
		renderTick(_appWindow.get(), ctx);
}

void	Engine::updateTick(FrameContext &frameCtx) {
	UiContext::newFrame();
	for (auto &func: _systems.getUpdates())
		func->execute(frameCtx);
	UiContext::endFrame();
}

void	Engine::renderTick(Window *window, FrameContext &ctx) {
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
		ctx.request = &renderRequest;
		_config.updateGlobalData(&_registry, ctx);

		for (auto &funcList: _systems.getRenders(renderRequest.requestType))
			executePass(ctx, funcList);

		for (auto &level: DrawQueue::flush()) {
			for (auto &pass: level.second) {
				if (auto renderer = RenderPass(*_device, ctx, _imagePool.get(),
										pass.dep)
									.beginPass()) {
					_frame.writeGlobalData(renderer.frameContext());
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

void	Engine::executePass(FrameContext &ctx, const SystemManager::EntryVec &funcs) {
	if (funcs.empty())
		return ;
	if (auto renderer = RenderPass(*_device, ctx, _imagePool.get(), funcs)
						.beginPass()) {
		_frame.writeGlobalData(renderer.frameContext());
		for (auto &func: funcs)
			func->execute(renderer);
	}
}

}
