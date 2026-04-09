/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/09 18:41:51                                        */
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
#include "ecs/Component.hpp"
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

void	Selection::configureTintPipeline(PipelineConfig &config) {
	config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

	config.depthStencilInfo.stencilTestEnable = VK_TRUE;
	config.depthStencilInfo.front.compareOp = VK_COMPARE_OP_EQUAL;
	config.depthStencilInfo.front.passOp = VK_STENCIL_OP_KEEP;
	config.depthStencilInfo.front.reference = 1;
	config.depthStencilInfo.front.compareMask = 0xFF;
	config.depthStencilInfo.front.writeMask = 0xFF;

	config.depthStencilInfo.depthTestEnable  = VK_FALSE;
	config.depthStencilInfo.depthWriteEnable = VK_FALSE;

	config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;

	VkPipelineColorBlendAttachmentState	attachment{};
	attachment.colorWriteMask = 0xF;
	attachment.blendEnable = VK_TRUE;
	attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	attachment.colorBlendOp = VK_BLEND_OP_ADD;
	attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	Pipeline::setBlendAttachment(config, 0, attachment);
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

void	Selection::configureEntityPipeline(PipelineConfig &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);
}

void	Selection::renderEntityID(const Renderer &renderer) {
	auto	ctx = renderer.frameContext();
	if (!ctx.commandBuffer)
		return ;

	auto	set = _registry->buildComponentSet<comp::Transform>(*_device, ctx.descriptorPool);
	if (!set)
		return ;
	auto	entities = _registry->view<include<comp::Transform, comp::Model>>();
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
		VkExtent2D	imgExtent = ctx.request->mainImage->getExtent();
		glm::vec2	viewportSize(imgExtent.width, imgExtent.height);
		auto	pos = glm::vec2(_inputState->getMousePos() - viewportOrigin);
		glm::vec4	viewport(viewportOrigin, viewportSize);
		if (pos.x < 0 || pos.y < 0 || pos.x > viewportSize.x || pos.y > viewportSize.y)
			return ;

		auto	entityImg = ctx.request->secondaryImages["entityID"];
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
