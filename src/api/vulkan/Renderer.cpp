/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:49:04 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/27 17:34:48                                        */
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

namespace	hel {

uint32_t	RenderPass::_passIndex = 0;
PipelineMap	*Renderer::Draw::_lastPipeline = VK_NULL_HANDLE;

RenderPass::RenderPass(Device &device, VkCommandBuffer commandBuffer,
						VkExtent2D extent)
	:	_device{device},
		_commandBuffer{commandBuffer},
		_extent{extent} {
}

RenderPass::RenderPass(RenderPass &&other)
	:	_device{other._device},
		_commandBuffer{other._commandBuffer},
		_isValid{other._isValid} {
	other._commandBuffer = VK_NULL_HANDLE;
}

RenderPass::~RenderPass(void) {
	if (_commandBuffer)
		endPass();
}

Renderer	RenderPass::beginPass(FrameContext &frameContext) {
	if (_colorsWrite.empty())
		return (Renderer(frameContext, std::move(*this)));

	VkRenderingInfo	renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea = {{0, 0}, _extent};
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = static_cast<uint32_t>(_colorsInfo.size());
	renderingInfo.pColorAttachments = _colorsInfo.data();
	if (_depthInfo.has_value()) {
		renderingInfo.pDepthAttachment = &(*_depthInfo);
		//TODO -> add it's own function to set it.
		renderingInfo.pStencilAttachment = &(*_depthInfo);
	}

	vkCmdBeginRendering(_commandBuffer, &renderingInfo);
	setViewport();
	_isValid = true;
	return (Renderer(frameContext, std::move(*this)));
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
	_colorsWrite.push_back(color);
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
	if (_pass._isValid)
		_frameContext.passIndex = RenderPass::newPass();
}

Renderer::operator	bool(void) const {
	return (_pass._isValid);
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
											uint32_t stride, uint32_t *retOffset) {
	_sets.push_back(set);
	uint32_t	alignement = _device->getPhysProperties().properties.limits
										.minUniformBufferOffsetAlignment;
	uint32_t	offset = ((stride + alignement - 1) & ~(alignement - 1)) * _frameContext->passIndex;
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
							pipelineLayout, 0, _sets.size(), _sets.data(),
							_setsOffsets.size(), _setsOffsets.data());
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
