/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Render.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/18 10:54:23 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/20 17:06:25                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Render.hpp"
#include "platform/window/Window.hpp"
#include "api/vulkan/Device.hpp"
#include "api/vulkan/Buffer.hpp"
#include "ecs/Registry.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Component.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/assets/Shader.hpp"
#include "core/Engine.hpp"

namespace	hel::sys {

Render::Render(Device &device, Registry &registry,
						VkDescriptorSetLayout &setLayout)
	:	ISystem(device, registry, setLayout),
		_assetManager{registry.getAssetManager()} {
}

Render::~Render(void) {
	if (_pipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(_device.getLogical(), _pipelineLayout, nullptr);
}

void	Render::render(VkRenderPass renderPass, WindowResources &resources, uint32_t currentFrame) {
	auto	commandBuffer = resources.commandBuffers[currentFrame];
	auto	pipeline = getPipelineForPass(renderPass);
	if (pipeline == nullptr || commandBuffer == VK_NULL_HANDLE)
		return ;

	pipeline->_pipeline.bind(commandBuffer);

	auto	entities = _registry.view<comp::Transform, comp::Model>();
	for (auto entity: entities) {
		auto	mesh = _assetManager.get<Geometry>(entities.get<comp::Model>(entity)->filePath);
		if (!mesh)	{ continue ; }
		auto	*transform = entities.get<comp::Transform>(entity);
		PushConstantData	push{transform->worldMatrix, transform->normalMatrix};

		vkCmdPushConstants(commandBuffer, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
							0, sizeof(PushConstantData), &push);
		VkBuffer	buffers[] = {mesh->vertexBuffer->getBuffer()};
		VkDeviceSize	offset[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);
		vkCmdBindIndexBuffer(commandBuffer, mesh->triangleIndexBuffer->getBuffer(), 0,
							VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
							_pipelineLayout, 0, 1,
							&resources.globalDescriptorSets[currentFrame], 0,
							nullptr);
		vkCmdDrawIndexed(commandBuffer, mesh->triangleVertexCount, 1, 0, 0, 0);
	}
}

Render::SystemPipeline	*Render::getPipelineForPass(VkRenderPass renderPass) {
	if (_pipelines.find(renderPass) != _pipelines.end())
		return (_pipelines[renderPass].get());
	auto	pipeline = std::make_unique<SystemPipeline>(*this);
	if (pipeline->init(renderPass))
		return (nullptr);
	_pipelines[renderPass] = std::move(pipeline);
	return (_pipelines[renderPass].get());
}

Render::SystemPipeline::SystemPipeline(Render &system)
	:	_pipeline{system._device},
		_system{system} {
}

Render::SystemPipeline::~SystemPipeline(void) {
	_pipeline.deleteGraphicsPipeline();
}

bool	Render::SystemPipeline::init(VkRenderPass renderPass) {
	auto	pipelineLayout = _system.getPipelineLayout();
	if (pipelineLayout == nullptr)
		return (true);
	hel::PipelineConfigInfo	configInfo{};
	Pipeline::defaultPipelineConfigInfo(configInfo);
	Pipeline::setVertexInputDescriptions<Vertex>(configInfo);
	configInfo.renderPass = renderPass;
	configInfo.pipelineLayout = pipelineLayout;

	auto	vert = _system._assetManager.get<Shader>(_system._vertPath);
	auto	frag = _system._assetManager.get<Shader>(_system._fragPath);
	if (!vert || !frag)
		return (true);
	if (_pipeline.createGraphicsPipeline(configInfo, {vert->getStageInfo(), frag->getStageInfo()}))
		return (true);
	return (false);
}

VkPipelineLayout	Render::getPipelineLayout(void) {
	if (_pipelineLayout != VK_NULL_HANDLE)
		return (_pipelineLayout);

	VkPushConstantRange	pushConstant{};
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstant.size = sizeof(PushConstantData);

	VkPipelineLayoutCreateInfo	layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &_setLayout;
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &pushConstant;

	if (vkCreatePipelineLayout(_device.getLogical(), &layoutInfo, nullptr, &_pipelineLayout))
		return (nullptr);
	return (_pipelineLayout);
}

}
