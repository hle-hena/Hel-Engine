/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/03 19:17:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/Selection.hpp"
#include "core/Queues.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "platform/window/Window.hpp"
#include <cstdint>
#include "core/SystemManager.hpp"

namespace	hel::sys {

SystemRegistrar<Selection>	reg_SelectionSystem;

void	Selection::init(void) {
	updateDeps.provides = "select entity";

	renderInterDeps.provides = "render stencil on selected entity";

	postProcessDeps.provides = "render color overlay on selected entity";
	postProcessDeps.write.push_back(ImageDep()
		.setImageName("mainColor")
		.setImageUsage(ImageDep::Usage::Color)
		.setFormatAsked(VK_FORMAT_B8G8R8A8_SRGB));
	postProcessDeps.write.push_back(ImageDep()
		.setImageName("depth layer")
		.setImageUsage(ImageDep::Usage::DepthStencil)
		.setFormatAsked(VK_FORMAT_D32_SFLOAT_S8_UINT));

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
		config.configurePipeline = configurePipeline;
		_tintPipeline = createPipeline(config);
	}
}

void	Selection::configurePipeline(PipelineConfig &config) {
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
	VkExtent2D	imgExtent = ctx.request->images["mainColor"]->getExtent();
	glm::vec2	viewportSize(imgExtent.width, imgExtent.height);
	auto	pos = glm::vec2(_inputState->getMousePos() - viewportOrigin);
	glm::vec4	viewport(viewportOrigin, viewportSize);
	if (pos.x < 0 || pos.y < 0 || pos.x > viewportSize.x || pos.y > viewportSize.y)
		return ;

	auto	entityImg = ctx.request->images["entityID"];
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
		.setVertexCount(3)
		.submit();
}

}
