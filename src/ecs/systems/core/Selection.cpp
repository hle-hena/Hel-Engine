/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/01 21:56:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Selection.hpp"
#include "ecs/Registry.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/AssetManager.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/ImagePool.hpp"

namespace	hel::sys {

void	Selection::init(void) {
	{
		_assetManager = &_registry->getAssetManager();
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = _assetManager;
		config.shaderPaths = {
			"assets/shaders/tint.vert.spv",
			"assets/shaders/tint.frag.spv"
		};
		config.configurePipeline = configureTintPipeline;
		_tintPipeline = createPipeline(config);
		_inputState = &_registry->getInputState();
	}
	{
		_assetManager = &_registry->getAssetManager();
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = &_registry->getAssetManager();
		config.shaderPaths = {
			"assets/shaders/basic.vert.spv",
			"assets/shaders/OutEntityID.frag.spv"
		};
		config.initPipelineLayout = initEntityLayout;
		config.configurePipeline = configureEntityPipeline;
		_entityIDPipeline = createPipeline(config);
	}
}

void	Selection::configureTintPipeline(PipelineConfigInfo &config) {
	config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

	config.depthStencilInfo.stencilTestEnable = VK_TRUE;
	config.depthStencilInfo.front.compareOp = VK_COMPARE_OP_EQUAL;
	config.depthStencilInfo.front.passOp = VK_STENCIL_OP_KEEP;
	config.depthStencilInfo.front.reference = 1;
	config.depthStencilInfo.front.compareMask = 0xFF;
	config.depthStencilInfo.front.writeMask = 0xFF;

	config.depthStencilInfo.depthTestEnable  = VK_FALSE;
	config.depthStencilInfo.depthWriteEnable = VK_FALSE;

	config.colorBlendAttachment.blendEnable = VK_TRUE;
	config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void	Selection::initEntityLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
						std::vector<VkPushConstantRange> &pushConstants) {
	VkPushConstantRange	vertexPush{};
	vertexPush.stageFlags = VK_SHADER_STAGE_ALL;
	vertexPush.size = sizeof(EntityData);
	pushConstants.push_back(vertexPush);
	setLayouts.push_back(DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.getSetLayout());
}

void	Selection::configureEntityPipeline(PipelineConfigInfo &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);
}

void	Selection::renderEntityID(const Renderer &renderer) {
	auto	ctx = renderer.frameContext();
	if (!ctx.commandBuffer)
		return ;

	auto	set = _registry->buildComponentSet<comp::Transform>(*_device, ctx.descriptorPool);
	if (!set)
		return ;
	auto	entities = _registry->view<comp::Transform, comp::Model>();
	for (auto entity: entities) {
		auto	mesh = _assetManager->get<Geometry>(entities.get<comp::Model>(entity)->filePath);
		if (!mesh)	{ continue ; }
		auto	transform = entities.get<comp::Transform>(entity);

		drawCommand(renderer, _entityIDPipeline)
			.addPush(VK_SHADER_STAGE_ALL, EntityData{entity, transform.getDenseIndex()})
			.addBinding(set->sets[0])
			.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
			.addIndexBuffer(mesh->triangleIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32)
			.submit(mesh->triangleVertexCount);
	}
}

void	Selection::checkSelectionResult(const FrameContext &ctx) {
	if (!_needReadback || ctx.frameIndex != _frameRequested)	{ return ; }

	uint32_t	*data = static_cast<uint32_t *>(_buff->getMapped());

	ctx.window->setEntityFocus(data[0]);
	_buff = nullptr;
	_needReadback = false;
}

void	Selection::updateWindow(const FrameContext &ctx) {
	if (_inputState->isPressed<input::Mouse>(0)) {
		auto	camera = _registry->getComponent<comp::Camera>(ctx.request->handle);
		auto	transform = _registry->getComponent<comp::Transform>(ctx.request->handle);
		if (!camera || !transform)
			return ;
		glm::vec2	viewportOrigin(ctx.request->origin.x, ctx.request->origin.y);
		VkExtent2D	imgExtent = ctx.request->img->getExtent();
		glm::vec2	viewportSize(imgExtent.width, imgExtent.height);
		auto	pos = glm::vec2(_inputState->getMousePos() - viewportOrigin);
		glm::vec4	viewport(viewportOrigin, viewportSize);
		if (pos.x < 0 || pos.y < 0 || pos.x > viewportSize.x || pos.y > viewportSize.y)
			return ;

		Image	*entityImg = _imagePool->acquire(Image::Config()
							.setWidth(imgExtent.width)
							.setHeight(imgExtent.height)
							.setFormats({VK_FORMAT_R32_UINT})
							.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
							.setUsage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
							.setAspect(VK_IMAGE_ASPECT_COLOR_BIT));
		auto	depthImage = _imagePool->acquire(Image::Config()
			.setFormats(VK_FORMAT_D32_SFLOAT_S8_UINT)
			.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
		FrameContext	renderCtx = ctx;
		VkClearValue	clear{};
		clear.color.uint32[0] = 0xFFFFFFFF;
		if (auto renderer = RenderPass(*_device, renderCtx.commandBuffer, imgExtent)
							.setClearValue(clear)
							.addColorWrite(entityImg, VK_FORMAT_R32_UINT)
							.addDepthWrite(depthImage, depthImage->getFormat())
							.beginPass(renderCtx))
			renderEntityID(renderer);

		_buff = Buffer::create(*_device, sizeof(uint32_t), 1,
							VK_BUFFER_USAGE_TRANSFER_DST_BIT,
							VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
							VMA_ALLOCATION_CREATE_MAPPED_BIT |
							VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
		VkOffset3D	mouseOffset = { (int32_t)pos.x, (int32_t)pos.y, 0 };
		VkExtent3D	pixelExtent = { 1, 1, 1 };
		entityImg->copyTo(ctx.commandBuffer, _buff.get(), mouseOffset, pixelExtent);
		_needReadback = true;
		_frameRequested = ctx.frameIndex;
	} else
		checkSelectionResult(ctx);
}

void	Selection::postProcessing(const Renderer &renderer) {
	drawCommand(renderer, _tintPipeline)
		.submitNoVertex(3);
}

}
