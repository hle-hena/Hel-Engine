/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 19:49:04 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/06 22:06:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

# include "api/vulkan/Renderer.hpp"
# include "api/vulkan/Image.hpp"

namespace	hel {

Renderer::Renderer(VkCommandBuffer commandBuffer, VkExtent2D extent)
	:	_commandBuffer{commandBuffer},
		_extent{extent} {
}

Renderer::Renderer(Renderer &&other)
	:	_commandBuffer{other._commandBuffer},
		_isValid{other._isValid} {
	other._commandBuffer = VK_NULL_HANDLE;
}

Renderer::~Renderer(void) {
	if (_commandBuffer)
		endPass();
}

RendererHandle	Renderer::beginPass(void) {
	if (_colors.empty())
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

Renderer	&Renderer::addColor(Image *color, VkFormat format) {
	color->transitionLayout(_commandBuffer,
							VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	_colorsInfo.push_back(color->getRenderingInfo(_colorClear, _colorsLoadOp,
												_colorsStoreOp, format));
	_colors.push_back(color);
	return (*this);
}

Renderer	&Renderer::addDepth(Image *depth, VkFormat format) {
	depth->transitionLayout(_commandBuffer,
							VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	_depthInfo = depth->getRenderingInfo(_depthClear, _depthLoadOp,
										_depthStoreOp, format);
	return (*this);
}

RendererHandle::RendererHandle(Renderer &&renderer)
	:	_renderer{std::move(renderer)} {
}

RendererHandle::operator	bool(void) const {
	return (_renderer._isValid);
}

}
