/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:49:04 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/05 16:08:26                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

# include "api/vulkan/Renderer.hpp"
# include "api/vulkan/Image.hpp"
# include "api/vulkan/Device.hpp"
# include "core/Frame.hpp"
# include "ecs/systems/ISystem.hpp"
# include "api/vulkan/ImagePool.hpp"
# include "core/Queues.hpp"

namespace	hel {

uint32_t	RenderPass::_passIndex = 0;
PipelineMap	*Renderer::Draw::_lastPipeline = VK_NULL_HANDLE;

RenderPass::RenderPass(Device &device, FrameContext &ctx,
						VkExtent2D extent)
	:	_device{device},
		_ctx{ctx},
		_req{ctx.request},
		_commandBuffer{ctx.commandBuffer},
		_extent{extent} {
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

bool	RenderPass::addWrite(ImageDep &dep, ImagePool *imagePool) {
	if (_writes.contains(dep.imageName)) {
		return (false);
	}
	if (_reads.contains(dep.imageName)) {
		std::cerr << "Error for image \"" << dep.imageName
			<< "\". Can't have an image be both a write and a read.\n";
		return (true);
	}

	if (dep.format == VK_FORMAT_MAX_ENUM) {
		std::cerr << "Error for image \"" << dep.imageName << "\". No format"
			<< " was asked. Use the setFormatAsked function.\n";
		return (true);
	}
	if (dep.usage == ImageDep::Usage::MAX_ENUM && findUsage(dep))
		return (true);
	if ((dep.load == VK_ATTACHMENT_LOAD_OP_MAX_ENUM) && findLoadOp(dep))//TODO
		return (true);
	if ((dep.store == VK_ATTACHMENT_STORE_OP_MAX_ENUM) && findStoreOp(dep))//TODO
		return (true);
	Image	*img = nullptr;
	if (_req->images.contains(dep.imageName))
		img = _req->images[dep.imageName];
	else if (dep.config.has_value()) {
		img = imagePool->acquire(dep.config.value());
		_req->images[dep.imageName] = img;
	} else {
		std::cerr << "Error for image \"" << dep.imageName << "\". Image"
			<< " doesn't exists, and no config has "
			<< "been provided in the dependancy.\n";
		return (true);
	}
	addWriteImage(img, dep);
	return (false);
}

bool	RenderPass::findLoadOp(ImageDep &dep) {
	if (!_req->images.contains(dep.imageName))
		dep.load = VK_ATTACHMENT_LOAD_OP_CLEAR;
	else
	 	dep.load = VK_ATTACHMENT_LOAD_OP_LOAD;
	return false;
}

bool	RenderPass::findStoreOp(ImageDep &dep) {
	dep.store = VK_ATTACHMENT_STORE_OP_STORE;
	return false;
}

bool	RenderPass::findUsage(ImageDep &dep) {
	if (!dep.config.has_value()) {
		std::cerr << "Couldn't find a valid usage for the image \"" <<
			dep.imageName << "\". Please use the setImageUsage function.\n";
		return true;
	}

	if (dep.config->usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		dep.usage = ImageDep::Usage::Color;
	else if (dep.config->usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		dep.usage = ImageDep::Usage::DepthStencil;
	else {
		std::cerr << "Couldn't find a valid usage for the image \"" <<
			dep.imageName << "\". Please use the setImageUsage function.\n";
		return true;
	}
	return false;
}

void	RenderPass::addWriteImage(Image *img, ImageDep &dep){
	_writes[dep.imageName] = img;

	if (dep.usage & ImageDep::Color) {
		img->transitionLayout(_commandBuffer,
								VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		_colorsInfo.push_back(img->getRenderingInfo(
			dep.clear.value_or(VkClearValue{.color = {{0.f, 0.f, 0.f, 1.f}}}),
			dep.load, dep.store, dep.format));
		_config.colorFormats.push_back(dep.format);
	}
	if (dep.usage & ImageDep::Usage::Depth) {
		img->transitionLayout(_commandBuffer,
						VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		_depthInfo = img->getRenderingInfo(
			dep.clear.value_or(VkClearValue{.depthStencil = {1.f, 0}}),
			dep.load, dep.store, dep.format);
		_config.depthFormat = dep.format;
	}
	if (dep.usage & ImageDep::Usage::Stencil) {
		img->transitionLayout(_commandBuffer,
						VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		_stencilInfo = img->getRenderingInfo(
			dep.clear.value_or(VkClearValue{.depthStencil = {1.f, 0}}),
			dep.load, dep.store, dep.format);
		_config.stencilFormat = dep.format;
	}
}

bool	RenderPass::addRead(ImageDep &dep) {
	auto	matchName = [](const std::string_view& pattern, const std::string_view& name) {
		if (pattern.ends_with('*'))
			return name.starts_with(pattern.substr(0, pattern.size() - 1));
		return pattern == name;
	};

	bool	notFound = true;
	for (auto &[imageName, image]: _req->images) {
		if (matchName(dep.imageName, imageName)) {
			if (_writes.contains(imageName)) {
				std::cerr << "Trying to read the image \"" << imageName
					<< "\" which is already registered as a write.\n";
				continue ;
			}
			if (_reads.contains(imageName))
				continue ;

			_reads[imageName] = image;
			image->transitionLayout(_commandBuffer,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			notFound = false;
		}
	}
	return (notFound);
}

Renderer	RenderPass::beginPass(void) {
	if (_invalidDep || _writes.empty())
		return (Renderer(_ctx, std::move(*this)));

	VkRenderingInfo	renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea = {{0, 0}, _extent};
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = static_cast<uint32_t>(_colorsInfo.size());
	renderingInfo.pColorAttachments = _colorsInfo.data();
	if (_depthInfo.has_value())
		renderingInfo.pDepthAttachment = &_depthInfo.value();
	if (_stencilInfo.has_value())
		renderingInfo.pStencilAttachment = &_stencilInfo.value();

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

RenderPass	&RenderPass::addColorWrite(Image *color, VkFormat format) {
	color->transitionLayout(_commandBuffer,
							VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	_colorsInfo.push_back(color->getRenderingInfo(_colorClear, _colorsLoadOp,
												_colorsStoreOp, format));
	_writes["OuiOuiHardCode"] = color;
	_config.colorFormats.push_back(format);
	return (*this);
}

RenderPass	&RenderPass::addDepthWrite(Image *depth, VkFormat format) {
	depth->transitionLayout(_commandBuffer,
							VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	_depthWrite = depth;
	_depthInfo = depth->getRenderingInfo(_depthClear, _depthLoadOp,
										_depthStoreOp, format);
	_config.depthFormat = format;
	return (*this);
}



Renderer::Renderer(FrameContext &frameContext, RenderPass &&pass)
	:	_device{pass._device},
		_frameContext{frameContext},
		_commandBuffer{pass._commandBuffer},
		_config{pass._config},
		_pass{std::move(pass)} {
	if (_pass._passStarted)
		_frameContext.passIndex = RenderPass::newPass();
}

Renderer::operator	bool(void) const {
	return (_pass._passStarted);
}

FrameContext	&Renderer::frameContext(void) const	{
	return (_frameContext);
}

uint32_t	Renderer::passIndex(void) const	{
	return (_frameContext.passIndex);
}

Renderer::Draw	Renderer::drawCommand(PipelineMap *pipeline, ISystemKey) const {
	Draw	drawCall(this, pipeline);
	drawCall.addDynamicBinding(_frameContext.globalSet, sizeof(GlobalUBO), nullptr);
	return (drawCall);
}

Renderer::Draw::Draw(const Renderer *renderer, PipelineMap *pipeline)
	:	_pipeline(pipeline),
		_device(&renderer->_device),
		_frameContext(&renderer->_frameContext),
		_commandBuffer(renderer->_commandBuffer),
		_config(renderer->_config) {}

Renderer::Draw	&Renderer::Draw::addIndexBuffer(VkBuffer buffer, uint32_t firstIndex,
							VkIndexType indexType, VkDeviceSize offset) {
	if (_hasIndex)
		return (*this);
	_indexInfos.buffer = buffer;
	_indexInfos.offset = offset;
	_indexInfos.indexType = indexType;
	_hasIndex = true;
	_firstIndex = firstIndex;
	return (*this);
}

Renderer::Draw	&Renderer::Draw::addBinding(VkDescriptorSet set) {
	_sets.push_back(set);
	return (*this);
}

Renderer::Draw	&Renderer::Draw::addDynamicBinding(VkDescriptorSet set,
											uint32_t stride, uint32_t *retOffset, VkBufferUsageFlags setUsage) {
	_sets.push_back(set);
	uint32_t	alignement = _device->getAligned(stride, setUsage);
	uint32_t	offset = alignement * _frameContext->passIndex;
	if (retOffset)
		(*retOffset = offset);
	_setsOffsets.push_back(offset);
	return (*this);
}

void	Renderer::Draw::submit(void) {
	if (!_count.has_value())
		return ;
	if (_lastPipeline != _pipeline)
		_pipeline->bindPipeline(_config, _commandBuffer);
	auto	pipelineLayout = _pipeline->getLayout();
	if (_hasPush) {
		vkCmdPushConstants(_commandBuffer, pipelineLayout,
				_pushInfos.stage, 0, _pushInfos.structSize, _pushInfos.data);
	}
	if (_hasVertex) {
		vkCmdBindVertexBuffers(_commandBuffer, 0, _vertexInfos.bufferCount,
							_vertexInfos.buffers, _vertexInfos.offsets);
	}
	vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout, 0, static_cast<uint32_t>(_sets.size()), _sets.data(),
		static_cast<uint32_t>(_setsOffsets.size()), _setsOffsets.data());
	if (_hasIndex) {
		vkCmdBindIndexBuffer(_commandBuffer, _indexInfos.buffer,
							_indexInfos.offset, _indexInfos.indexType);
		vkCmdDrawIndexed(_commandBuffer, _count.value(), 1,
			_firstIndex, 0, 0);
	}
	else
		vkCmdDraw(_commandBuffer, _count.value(), 1, 0, 0);
}

}
