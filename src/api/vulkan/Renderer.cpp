/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:49:04 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/23 17:01:29                                        */
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

# include <iostream>

namespace	hel {

Renderer::Renderer(Device &device, VkCommandBuffer commandBuffer, VkExtent2D extent)
	:	_device{device},
		_commandBuffer{commandBuffer},
		_extent{extent} {
}

Renderer::Renderer(Renderer &&other)
	:	_device{other._device},
		_commandBuffer{other._commandBuffer},
		_config{other._config},
		_isValid{other._isValid} {
	other._commandBuffer = VK_NULL_HANDLE;
}

Renderer::~Renderer(void) {
	if (_commandBuffer)
		endPass();
}

RendererHandle	Renderer::beginPass(void) {
	if (_colorsWrite.empty())
		return (RendererHandle(std::move(*this)));

	VkRenderingInfo	renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea = {{0, 0}, _extent};
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = _colorsInfo.size();
	renderingInfo.pColorAttachments = _colorsInfo.data();
	if (_depthInfo.has_value())
		renderingInfo.pDepthAttachment = &(*_depthInfo);

	vkCmdBeginRendering(_commandBuffer, &renderingInfo);
	setViewport();
	_isValid = true;
	return (RendererHandle(std::move(*this)));
}

void	Renderer::setViewport(void) {
	VkViewport	viewport{};
	viewport.height = static_cast<float>(_extent.height);
	viewport.width = static_cast<float>(_extent.width);
	viewport.maxDepth = 1.f;
	vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

	VkRect2D	scissor{};
	scissor.extent = _extent;
	vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);
}

void	Renderer::endPass(void) {
	vkCmdEndRendering(_commandBuffer);
}

Renderer	&Renderer::addColorWrite(Image *color, VkFormat format) {
	color->transitionLayout(_commandBuffer,
							VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	_colorsInfo.push_back(color->getRenderingInfo(_colorClear, _colorsLoadOp,
												_colorsStoreOp, format));
	_colorsWrite.push_back(color);
	_config.colorFormats.push_back(format);
	return (*this);
}

Renderer	&Renderer::addDepthWrite(Image *depth, VkFormat format) {
	depth->transitionLayout(_commandBuffer,
							VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	_depthWrite = depth;
	_depthInfo = depth->getRenderingInfo(_depthClear, _depthLoadOp,
										_depthStoreOp, format);
	_config.depthFormat = format;
	return (*this);
}

RendererHandle::RendererHandle(Renderer &&renderer)
	:	_renderer{std::move(renderer)} {
	_config = _renderer._config;
}

RendererHandle::operator	bool(void) const {
	return (_renderer._isValid);
}

bool	RendererHandle::bindPipeline(PipelineMap *pipeline, ISystemKey) const {
	return (pipeline->bindPipeline(_config, _renderer._commandBuffer));
}

RendererHandle::Draw	RendererHandle::drawCommand(VkPipelineLayout layout, ISystemKey) const {
	Draw	drawCall {_renderer._device, _renderer._commandBuffer, layout};
	return (drawCall);
}

RendererHandle::Draw	&RendererHandle::Draw::addIndexBuffer(VkBuffer buffer, VkDeviceSize offset,
							VkIndexType indexType, uint32_t firstIndex) {
	if (_hasIndex)
		return (*this);
	vkCmdBindIndexBuffer(_commandBuffer, buffer, offset, indexType);
	_hasIndex = true;
	_firstIndex = firstIndex;
	return (*this);
}

RendererHandle::Draw	&RendererHandle::Draw::addBinding(VkDescriptorSet set) {
	_sets.push_back(set);
	return (*this);
}

RendererHandle::Draw	&RendererHandle::Draw::addBinding(VkDescriptorSet set,
														uint32_t stride) {
	_sets.push_back(set);
	uint32_t	alignement = _device.getPhysProperties().properties.limits
										.minUniformBufferOffsetAlignment;
	uint32_t	offset = (stride + alignement - 1) & ~(alignement - 1);
	_setsOffsets.push_back(offset);
	return (*this);
}

void	RendererHandle::Draw::submit(uint32_t indexCount, uint32_t instanceCount,
				uint32_t firstInstance) {
	if (!_hasVertex)
		return ;
	vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
							_pipelineLayout, 0, _sets.size(), _sets.data(),
							_setsOffsets.size(), _setsOffsets.data());
	if (_hasIndex)
		vkCmdDrawIndexed(_commandBuffer, indexCount, instanceCount,
						_firstIndex, 0, instanceCount);
	else
		vkCmdDraw(_commandBuffer, indexCount, instanceCount, 0, firstInstance);
}

}
