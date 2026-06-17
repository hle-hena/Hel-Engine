/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderPass.cpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/12 18:36:48 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/17 11:17:05                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/RenderPass.hpp"
#include "api/vulkan/ImagePool.hpp"
#include "core/Frame.hpp"
#include "core/Queues.hpp"
#include "core/SystemManager.hpp"

namespace	hel {

uint32_t	RenderPass::_passIndex = 0;

RenderPass::RenderPass(Device &device, FrameContext &ctx, ImagePool *imagePool,
			const SystemManager::FuncVec &funcs)
	:	_device{device},
		_ctx{ctx},
		_req{ctx.request},
		_commandBuffer{ctx.commandBuffer},
		_extent{ctx.request->images["mainColor"]->getExtent()} {
	for (auto &func: funcs) {
		for (auto &dep: func->getDep()->write)
			_invalidDep |= addWrite(dep, imagePool);
		for (auto &dep: func->getDep()->read)
			_invalidDep |= addRead(dep);
	}
}

RenderPass::RenderPass(Device &device, FrameContext &ctx, ImagePool *imagePool,
	PhaseDependencies dep)
	:	_device{device},
		_ctx{ctx},
		_req{ctx.request},
		_commandBuffer{ctx.commandBuffer},
		_extent{ctx.request->images["mainColor"]->getExtent()} {
	for (auto &dep: dep.write)
		_invalidDep |= addWrite(dep, imagePool);
	for (auto &dep: dep.read)
		_invalidDep |= addRead(dep);
}

RenderPass::RenderPass(RenderPass &&other)
	:	_device{other._device},
		_ctx{other._ctx},
		_req{other._ctx.request},
		_commandBuffer{other._commandBuffer},
		_passStarted{other._passStarted} {
	other._commandBuffer = VK_NULL_HANDLE;
}

RenderPass::~RenderPass(void) {
	if (_commandBuffer && _passStarted)
		endPass();
}

#define	RETURN_ERROR(error)	\
do {						\
	std::cerr << error;		\
	return true;			\
} while (0)


bool	RenderPass::addWrite(ImageDep &dep, ImagePool *imagePool) {
	if (resolveUsage(dep) || validateWrite(dep))
		return true;
	if (_writes.contains(dep._imageName))
		return false;

	Image	*img = nullptr;
	if (_req->images.contains(dep._imageName))
		img = _req->images[dep._imageName];
	else if (dep._config.has_value()) {
		img = imagePool->acquire(dep._config.value());
		_req->images[dep._imageName] = img;
	} else
		RETURN_ERROR("Error for image \"" << dep._imageName << "\". Image"
			<< " doesn't exists, and no config has "
			<< "been provided in the dependancy.\n");
	if (dep._load == VK_ATTACHMENT_LOAD_OP_MAX_ENUM ||
		dep._store == VK_ATTACHMENT_STORE_OP_MAX_ENUM)
		resolveOps(img, dep);
	addWriteImage(img, dep);
	return (false);
}

bool	RenderPass::resolveUsage(ImageDep &dep) {
	if (dep._usage != ImageDep::Usage::MAX_ENUM)
		return false;

	if (!dep._config.has_value())
		RETURN_ERROR("Couldn't find a valid usage for the image \"" <<
			dep._imageName << "\". Please use the setImageUsage function.\n");

	if (dep._config->usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		dep._usage = ImageDep::Usage::Color;
	else if (dep._config->usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		dep._usage = ImageDep::Usage::DepthStencil;
	else 
		RETURN_ERROR("Couldn't find a valid usage for the image \"" <<
			dep._imageName << "\". Please use the setImageUsage function.\n");
	return false;
}

bool	RenderPass::validateWrite(ImageDep &dep) {
	bool	colorUsage = dep._usage & ImageDep::Color;
	bool	depthUsage = dep._usage & ImageDep::DepthStencil;

	if (colorUsage && depthUsage)
		RETURN_ERROR("Error for image \"" << dep._imageName
				<< "\". Too many aspects.");
	if (_writes.contains(dep._imageName)) {
		auto	contains = [&](){
			auto	it = std::find_if(_colorInfos.begin(), _colorInfos.end(),
						[&](auto &it){return it.second.name == dep._imageName;});
			return it != _colorInfos.end();
		};

		if ((colorUsage && !contains()) || (!colorUsage && contains()))
			RETURN_ERROR("Error for image \"" << dep._imageName
				<< "\". Trying to use it as both depth/stencil and color.");
	} if (_reads.contains(dep._imageName))
		RETURN_ERROR("Error for image \"" << dep._imageName
			<< "\". Can't have an image be both a write and a read.\n");
	if (dep._format == VK_FORMAT_MAX_ENUM)
		RETURN_ERROR("Error for image \"" << dep._imageName <<
			"\". Please ask for a valid format.\n");
	if (colorUsage) {
		if (!dep._bindingIndex.has_value())
			RETURN_ERROR("Error for image \"" << dep._imageName <<
				"\". No binding index for the color write has been given.\n");
		auto	idx = dep._bindingIndex.value();
		if (_colorInfos.contains(idx) &&
			_colorInfos[idx].name != dep._imageName)
			RETURN_ERROR("Error for image \"" << dep._imageName
				<< "\". The binding " << std::to_string(idx)
				<< " is already taken by image \""
				<< _colorInfos[idx].name << "\".\n");
	}
	return false;
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
		ColorWrite	write{.name = dep._imageName, .format = dep._format,
			.info = img->getRenderingInfo(
				dep._clearValue.value_or(VkClearValue{{{0.f, 0.f, 0.f, 1.f}}}),
				dep._load, dep._store, img->getView(ViewConfig()
					.format(dep._format).aspect(VK_IMAGE_ASPECT_COLOR_BIT)
					.components().identity()))};
		_colorInfos[dep._bindingIndex.value()] = write;
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
	if (hasDepth)	{ _depthInfo = info; _config.depthFormat = dep._format; }
	if (hasStenc)	{ _stencilInfo = info; _config.stencilFormat = dep._format; }
}

bool	RenderPass::addRead(const std::string_view &readName) {
	auto	matchName = [](const std::string_view &pattern,
						const std::string_view &name)
	{
		if (pattern.ends_with('*'))
			return name.starts_with(pattern.substr(0, pattern.size() - 1));
		return pattern == name;
	};

	bool	notFound = true;
	for (auto &[imageName, image]: _req->images) {
		if (matchName(readName, imageName)) {
			if (_writes.contains(imageName)) {
				std::cerr << "Trying to read the image \"" << imageName
					<< "\" which is already registered as a write.\n";
				return (true);
			}
			if (_reads.contains(imageName))
				continue ;
			if (!image->wasWritten()) {
				std::cerr << "Trying to read the image \"" << imageName
					<< "\" which was never written.\n";
				return (true);
			}

			_reads[imageName] = image;
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
	int										lastIndex = -1;
	for (auto &[index, write]: _colorInfos) {
		if (index != lastIndex + 1) {
			std::cerr << "There is a gap in the binding of images";
			return (Renderer(_ctx, std::move(*this)));
		}
		colors.push_back(write.info);
		_config.colorFormats.push_back(write.format);
		lastIndex = index;
	}
	renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colors.size());
	renderingInfo.pColorAttachments = colors.data();
	if (_depthInfo.has_value())
		renderingInfo.pDepthAttachment = &_depthInfo.value();
	if (_stencilInfo.has_value())
		renderingInfo.pStencilAttachment = &_stencilInfo.value();

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
