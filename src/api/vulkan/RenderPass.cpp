/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderPass.cpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/19 17:29:49 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/19 18:49:59                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/RenderPass.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Swapchain.hpp"
#include "platform/window/Window.hpp"

namespace	hel {

RenderPass::RenderPass(Device &device)
	:	_device{device} {
}

RenderPass::~RenderPass(void) {
	for (auto elem : _renderPasses) {
		if (elem.second)
			vkDestroyRenderPass(_device.getLogical(), elem.second, nullptr);
	}
}

VkRenderPass	RenderPass::getRenderPasss(VkFormat imageFormat, VkFormat depthFormat) {
	//TODO -> make the depthFormat be dependant too.
	if (_renderPasses.find(imageFormat) == _renderPasses.end()) {
		auto	renderPass = createRenderPass(imageFormat, depthFormat);
		if (!renderPass)
			return (nullptr);
		_renderPasses[imageFormat] = renderPass;
	}
	return (_renderPasses[imageFormat]);
}

VkRenderPass	RenderPass::createRenderPass(VkFormat imageFormat, VkFormat depthFormat) {
	VkAttachmentDescription	colorAttachment{};
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.format = imageFormat;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkAttachmentDescription	depthAttachment{};
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.format = depthFormat;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	VkAttachmentReference	colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference	depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	VkSubpassDescription	subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	VkSubpassDependency	dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
							VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
							VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
								VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2>	attachments = {colorAttachment, depthAttachment};
	VkRenderPassCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	createInfo.pAttachments = attachments.data();
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;

	VkRenderPass			renderPass;
	if (vkCreateRenderPass(_device.getLogical(), &createInfo, nullptr, &renderPass))
		return (nullptr);
	_renderPasses[imageFormat] = renderPass;
	return (renderPass);
}

void	RenderPass::beginRenderPass(VkRenderPass renderPass,
									VkCommandBuffer commandBuffer,
									VkFramebuffer frameBuffer,
									VkExtent2D extent) {
	VkRenderPassBeginInfo	renderPassBegin{};
	renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBegin.renderPass = renderPass;
	renderPassBegin.framebuffer = frameBuffer;
	renderPassBegin.renderArea.extent = extent;
	renderPassBegin.renderArea.offset = {0, 0};
	std::array<VkClearValue, 2>	clearValues{};
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};
	renderPassBegin.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBegin.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport	viewport{};
	viewport.height = static_cast<float>(extent.height);
	viewport.width = static_cast<float>(extent.width);
	viewport.maxDepth = 1.f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D	scissor{};
	scissor.extent = extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void	RenderPass::endRenderPass(VkCommandBuffer commandBuffer) {
	vkCmdEndRenderPass(commandBuffer);
}

}
