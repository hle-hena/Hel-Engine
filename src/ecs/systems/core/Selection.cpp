/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/14 11:30:53                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Selection.hpp"
#include "core/Queues.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Registry.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Component.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/ImagePool.hpp"
#include <cstdint>

namespace	hel::sys {

void	Selection::init(void) {
	_inputState = &_registry->getInputState();
	_assetManager = &_registry->getAssetManager();
	{
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = _assetManager;
		config.shaderPaths = {
			"assets/shaders/tint.vert.spv",
			"assets/shaders/tint.frag.spv"
		};
		config.configurePipeline = configureTintPipeline;
		_tintPipeline = createPipeline(config);
	}
	{
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

void	Selection::update(const FrameContext &ctx) {
	std::erase_if(_requests, [&](auto &item){
		auto	&[key, req] = item;
		if (req.frameIndex == ctx.frameIndex) {
			uint32_t	*data = static_cast<uint32_t *>(req.buffer->getMapped());

			Entity::id	handle = data[0];
			if (handle == Entity::NOT_REGISTERED)
				ctx.window->setEntityFocus(handle);
			else if (!_registry->getComponent<comp::NonSelectableTag>(handle)) {
				ctx.window->setEntityFocus(handle);
			}
			return (true);
		}
		return (false);
	});
}

void	Selection::renderInteraction(const Renderer &renderer) {
	auto	ctx = renderer.frameContext();
	if (!_inputState->isPressed<input::Mouse>(0))
		return ;
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
	if (!entityImg)
		return ;

	_requests.insert_or_assign(*ctx.request,
		Read::Queue::newRequest<uint32_t>(ctx.frameIndex)
			.setSrcImage(entityImg)
			.setOffset({(int32_t)pos.x, (int32_t)pos.y, 0})
			.setExtent({1, 1, 1})
			.push(*_device));
}

void	Selection::postProcessing(const Renderer &renderer) {
	drawCommand(renderer, _tintPipeline)
		.submitNoVertex(3);
}

}
