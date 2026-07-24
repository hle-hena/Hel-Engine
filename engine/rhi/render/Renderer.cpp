/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:49:04 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 10:42:51                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/render/Renderer.hpp"
#include "rhi/context/Device.hpp"

#include "core/Frame.hpp"//remove aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

namespace	hel {

PipelineMap	*DrawCall::_lastPipeline = VK_NULL_HANDLE;

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
	if (!_pass._passStarted)
		std::cout << "A pass didn't start.\n";
	return (_pass._passStarted);
}

FrameContext	&Renderer::frameContext(void) const	{
	return (_frameContext);
}

uint32_t	Renderer::passIndex(void) const	{
	return (_frameContext.passIndex);
}

DrawCall	Renderer::drawCommand(PipelineMap *pipeline, ISystemKey) const {
	DrawCall	drawCall(this, pipeline);
	drawCall.addDynamicBinding(_frameContext.globalSet, _frameContext.setStride, nullptr);
	return (drawCall);
}

DrawCall::DrawCall(const Renderer *renderer, PipelineMap *pipeline)
	:	_pipeline(pipeline),
		_device(&renderer->_device),
		_frameContext(&renderer->_frameContext),
		_commandBuffer(renderer->_commandBuffer),
		_config(renderer->_config) {}

DrawCall	&DrawCall::addIndexBuffer(VkBuffer buffer, uint32_t firstIndex,
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

DrawCall	&DrawCall::addBinding(VkDescriptorSet set) {
	_sets.push_back(set);
	return (*this);
}

DrawCall	&DrawCall::addDynamicBinding(VkDescriptorSet set,
											uint32_t stride, uint32_t *retOffset, VkBufferUsageFlags setUsage) {
	_sets.push_back(set);
	uint32_t	alignement = _device->getAligned(stride, setUsage);
	uint32_t	offset = alignement * _frameContext->passIndex;
	if (retOffset)
		(*retOffset = offset);
	_setsOffsets.push_back(offset);
	return (*this);
}

void	DrawCall::submit(void) {
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
