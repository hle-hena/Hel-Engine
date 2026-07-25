/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderPass.cpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/12 18:36:48 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/25 17:19:31                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/render/RenderPass.hpp"
#include "rhi/render/Renderer.hpp"
#include "rhi/resources/ImagePool.hpp"
#include "utils/str_utils.hpp"

#include "core/scheduler/PhaseDependency.hpp"//remove aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
#include "core/scheduler/RenderQueue.hpp"//remove aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
#include "core/scheduler/CycleEntry.hpp"//remove aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

#include <iostream>

namespace	hel {

uint32_t	RenderPass::_passIndex = 0;

RenderPass::RenderPass(Device &device, ExecutionContext &ctx, ImagePool *imagePool,
			const std::vector<sys::CycleEntry *> &funcs)
	:	_device{device},
		_ctx{ctx},
		_req{ctx.request},
		_commandBuffer{ctx.commandBuffer},
		_extent{ctx.request->images["mainColor"]->getExtent2D()} {
	for (auto &func: funcs) {
		for (auto &dep: func->getDep()->write)
			_invalidDep |= addWrite(dep, imagePool);
		for (auto &dep: func->getDep()->read)
			_invalidDep |= addRead(dep);
	}
}

RenderPass::RenderPass(Device &device, ExecutionContext &ctx, ImagePool *imagePool,
	PhaseDependencies deps)
	:	_device{device},
		_ctx{ctx},
		_req{ctx.request},
		_commandBuffer{ctx.commandBuffer},
		_extent{ctx.request->images["mainColor"]->getExtent2D()} {
	for (auto &dep: deps.write)
		_invalidDep |= addWrite(dep, imagePool);
	for (auto &dep: deps.read)
		_invalidDep |= addRead(dep);
}

RenderPass::RenderPass(RenderPass &&other)
	:	_device{other._device},
		_ctx{other._ctx},
		_req{other._ctx.request},
		_commandBuffer{other._commandBuffer},
		_extent{other._extent},
		_passStarted{other._passStarted} {
	other._commandBuffer = VK_NULL_HANDLE;
}

RenderPass::~RenderPass(void) {
	if (_commandBuffer && _passStarted)
		endPass();
}

bool	RenderPass::addWrite(ImageDep &dep, ImagePool *imagePool) {
	if (_writes.contains(dep._imageName))
		return false;

	Ref<Image>	img = nullptr;
	if (_req->images.contains(dep._imageName))
		img = _req->images[dep._imageName];
	else if (dep._config.has_value()) {
		img = imagePool->acquire(_ctx.frameIndex, dep._config.value());
		_req->images[dep._imageName] = img;
	} else {
		std::cerr << "Error for image \"" << dep._imageName << "\". Image"
			<< " doesn't exists, and no config has "
			<< "been provided in the dependancy.\n";
		return true;
	}
	if (dep._load == VK_ATTACHMENT_LOAD_OP_MAX_ENUM ||
		dep._store == VK_ATTACHMENT_STORE_OP_MAX_ENUM)
		resolveOps(img.get(), dep);
	addWriteImage(img.get(), dep);
	return (false);
}

void	RenderPass::resolveOps(Image *img, ImageDep &dep) {
	if (dep._load == VK_ATTACHMENT_LOAD_OP_MAX_ENUM) {
		if (img->wasWritten())
			dep._load = VK_ATTACHMENT_LOAD_OP_LOAD;
		else
			dep._load = VK_ATTACHMENT_LOAD_OP_CLEAR;
	}
	if (dep._store == VK_ATTACHMENT_STORE_OP_MAX_ENUM)
		dep._store = VK_ATTACHMENT_STORE_OP_STORE;
}

void	RenderPass::addWriteImage(Image *img, ImageDep &dep){
	_writes[dep._imageName] = img;

	if (dep._usage & ImageDep::Color) {
		img->transitionLayout(_commandBuffer,
								VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		Write	write{.name = dep._imageName, .format = dep._format,
			.info = img->getRenderingInfo(
				dep._clearValue.value_or(VkClearValue{{{0.f, 0.f, 0.f, 1.f}}}),
				dep._load, dep._store, img->getView(ViewConfig()
					.format(dep._format).aspect(VK_IMAGE_ASPECT_COLOR_BIT)
					.components().identity()))};
		_colorInfos[dep._bindingIndex] = write;
		return ;
	}

	bool				hasDepth = dep._usage & ImageDep::Depth;
	bool				hasStenc = dep._usage & ImageDep::Stencil;
	img->transitionLayout(_commandBuffer,
								VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	VkImageAspectFlags	aspect = 0;
	if (hasDepth)		aspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
	if (hasStenc)		aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
	auto	info = img->getRenderingInfo(
					dep._clearValue.value_or(VkClearValue{.depthStencil = {1.f, 0}}),
					dep._load, dep._store, img->getView(ViewConfig()
						.format(dep._format).aspect(aspect)
						.components().identity()));
	if (hasDepth)	{ _depthInfo = Write{dep._imageName, dep._format, info}; }
	if (hasDepth)	{ _stencilInfo = Write{dep._imageName, dep._format, info}; }
}

bool	RenderPass::addRead(const std::string_view &readName) {
	bool	notFound = true;
	for (auto &[imageName, image]: _req->images) {
		if (match(readName, imageName)) {
			if (_reads.contains(imageName))
				continue ;
			if (!image->wasWritten()) {
				std::cerr << "Trying to read the image \"" << imageName
					<< "\" which was never written.\n";
				return (true);
			}

			_reads[imageName] = image.get();
			image->transitionLayout(_commandBuffer,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			notFound = false;
		}
	}
	return (notFound);
}

Renderer	RenderPass::beginPass(void) {
	if (_invalidDep || _colorInfos.empty())
		return (Renderer(_ctx, std::move(*this)));

	VkRenderingInfo	renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.pNext = nullptr;
	renderingInfo.renderArea = {{0, 0}, _extent};
	renderingInfo.layerCount = 1;

	std::vector<VkRenderingAttachmentInfo>	colors;
	uint32_t	lastIndex = static_cast<uint32_t>(-1);
	for (auto &[index, write]: _colorInfos) {
		if (index != lastIndex + 1u) {
			std::cerr << "There is a gap in the binding of images.\n";
			return (Renderer(_ctx, std::move(*this)));
		}
		colors.push_back(write.info);
		_config.colorFormats.push_back(write.format);
		lastIndex = index;
	}
	renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colors.size());
	renderingInfo.pColorAttachments = colors.data();
	if (_depthInfo.has_value()) {
		renderingInfo.pDepthAttachment = &_depthInfo->info;
		_config.depthFormat = _depthInfo->format;
	} if (_stencilInfo.has_value()) {
		renderingInfo.pStencilAttachment = &_stencilInfo->info;
		_config.stencilFormat = _stencilInfo->format;
	}

	for (auto &[imgName, img]: _writes)
		img->setWrittenState({});

	vkCmdBeginRendering(_commandBuffer, &renderingInfo);
	setViewport();
	_passStarted = true;
	return (Renderer(_ctx, std::move(*this)));
}

void	RenderPass::setViewport(void) {
	VkViewport	viewport{};
	viewport.height = static_cast<float>(_extent.height);
	viewport.width = static_cast<float>(_extent.width);
	viewport.maxDepth = 1.f;
	vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

	VkRect2D	scissor{};
	scissor.extent = _extent;
	vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);
}

void	RenderPass::endPass(void) {
	vkCmdEndRendering(_commandBuffer);
}

}
