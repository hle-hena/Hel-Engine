/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Selection.cpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/25 10:31:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/07 18:31:10                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "systems/Selection.hpp"
#include "components/SelectionTag.hpp"
#include "components/Camera.hpp"
#include "components/Transform.hpp"

namespace	hel::sys {

SystemRegistrar<Selection>	reg_SelectionSystem;

void	Selection::init(void) {
	addUpdateDep("input/logic/selection", &Selection::update);

	addRenderDep("post-processing/selection read", &Selection::renderInteraction)
		->getDep()
			->addActiveLayer("RenderScene")
			->addRequire("render")
			->startWrite<Color>("mainColor", VK_FORMAT_B8G8R8A8_SRGB, 0)
				.addDep()
			->startWrite<Color>("entity layer", VK_FORMAT_R32_UINT, 1)
				.addDep()
			->startWrite<Depth>("gizmo depth layer", VK_FORMAT_D32_SFLOAT_S8_UINT)
				.config(Image::Config()
					.setFormats(VK_FORMAT_D32_SFLOAT_S8_UINT)
					.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
					.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT
							| VK_IMAGE_ASPECT_STENCIL_BIT))
				.addDep();

	addRenderDep("post-processing/selection color overlay", &Selection::postProcessing)
		->getDep()
			->addRequire("render/scene")
			->addBlock("render/gizmo")
			->addActiveLayer("RenderScene")
			->startWrite<Color>("mainColor", VK_FORMAT_B8G8R8A8_SRGB, 0)
				.addDep()
			->startWrite<Color>("entity layer", VK_FORMAT_R32_UINT, 1)
				.addDep()
			->startWrite<DepthStencil>("depth layer", VK_FORMAT_D32_SFLOAT_S8_UINT)
				.addDep();

	_assetManager = _registry->assetManager();
	{
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = _assetManager;
		config.shaderPaths = {
			"tint.vert",
			"tint.frag"
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
			else if (!_registry->getComponent<comp::NonSelectableTag>(handle))
				ctx.window->setEntityFocus(handle);
			return (true);
		}
		return (false);
	});

	if (ctx.window->focusChanged()) {
		if (_selectedEntity != Entity::NOT_REGISTERED)
			_registry->removeComponent<comp::SelectedTag>(_selectedEntity);
		_selectedEntity = ctx.window->getEntityFocus();
		if (_selectedEntity != Entity::NOT_REGISTERED)
			_registry->addComponent<comp::SelectedTag>(_selectedEntity);
	}
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

	auto	entityImg = ctx.request->images["entity layer"];
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
