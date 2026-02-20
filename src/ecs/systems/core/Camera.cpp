/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/20 10:17:43                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/Camera.hpp"
#include "api/vulkan/Device.hpp"
#include "ecs/AssetManager.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "ecs/assets/Geometry.hpp"
#include "ecs/assets/Shader.hpp"
#include "platform/window/Window.hpp"
#include "core/Engine.hpp"

namespace	hel::sys {

Camera::Camera(Device &device, Registry &registry, VkDescriptorSetLayout &setLayout)
	:	ISystem(device, registry, setLayout),
		_assetManager{registry.getAssetManager()} {
}

Camera::~Camera(void) {
	if (_pipelineLayout)
		vkDestroyPipelineLayout(_device.getLogical(), _pipelineLayout, nullptr);
}

Camera::SystemPipeline	*Camera::getPipelineForPass(VkRenderPass renderPass) {
	if (_pipelines.find(renderPass) != _pipelines.end())
		return (_pipelines[renderPass].get());
	auto	pipeline = std::make_unique<SystemPipeline>(*this);
	if (pipeline->init(renderPass))
		return (nullptr);
	_pipelines[renderPass] = std::move(pipeline);
	return (_pipelines[renderPass].get());
}

VkPipelineLayout	Camera::getPipelineLayout(void) {
	if (_pipelineLayout)
		return (_pipelineLayout);

	VkPushConstantRange	pushConstantRange{};
	pushConstantRange.size = sizeof(PushConstantData);
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount = 1;
	createInfo.pSetLayouts = &_setLayout;
	createInfo.pushConstantRangeCount = 1;
	createInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(_device.getLogical(), &createInfo, nullptr, &_pipelineLayout))
		return (nullptr);
	return (_pipelineLayout);
}

void	Camera::update(float deltaTime) {
	auto	entities = _registry.view<comp::Transform, comp::Camera>();

	for (auto entity: entities) {
		auto	*constTransform = entities.get<comp::Transform>(entity);
		auto	*constCamera = entities.get<comp::Camera>(entity);

		if (!constCamera->isDirty && !constTransform->isDirty)
			continue ;
		if (auto camera = _registry.modify(constCamera)) {

			glm::mat4 rotate = glm::mat4_cast(glm::conjugate(constTransform->rotation));
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), -constTransform->position);
			glm::mat4 view = rotate * translate;

			glm::mat4 projection = glm::perspective(camera->fov, camera->aspect, camera->near, camera->far);
			projection[1][1] *= -1; 

			camera->viewProjection = projection * view;
		}
	}
}

void	Camera::render(VkRenderPass renderPass, WindowResources &resources,
					uint32_t currentFrame) {
	auto	commandBuffer = resources.commandBuffers[currentFrame];
	auto	pipeline = getPipelineForPass(renderPass);
	if (pipeline == nullptr || commandBuffer == VK_NULL_HANDLE)
		return ;
	pipeline->_pipeline.bind(commandBuffer);

	auto	entities = _registry.view<comp::Camera,
									comp::Transform>();
	for (auto entity : entities) {
		auto	mesh = _assetManager.get<Geometry>("assets/models/colored_cube.obj");
		if (!mesh)	{ continue ; }
		auto	*transform = entities.get<comp::Transform>(entity);
		PushConstantData	push{transform->worldMatrix, transform->normalMatrix};

		vkCmdPushConstants(commandBuffer, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
							0, sizeof(PushConstantData), &push);
		VkBuffer	buffers[] = {mesh->vertexBuffer->getBuffer()};
		VkDeviceSize	offset[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);
		vkCmdBindIndexBuffer(commandBuffer, mesh->indexBuffer->getBuffer(), 0,
							VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
							_pipelineLayout, 0, 1,
							&resources.globalDescriptorSets[currentFrame], 0,
							nullptr);
		vkCmdDrawIndexed(commandBuffer, mesh->vertexCount, 1, 0, 0, 0);
	}
}



Camera::SystemPipeline::SystemPipeline(Camera &system)
	:	_pipeline {system._device},
		_system {system} {
}

Camera::SystemPipeline::~SystemPipeline(void) {
	_pipeline.deleteGraphicsPipeline();
}

bool	Camera::SystemPipeline::init(VkRenderPass renderPass) {
	auto	layout = _system.getPipelineLayout();
	if (!layout)
		return (true);
	PipelineConfigInfo	config{};
	Pipeline::defaultPipelineConfigInfo(config);
	Pipeline::setVertexInputDescriptions<Vertex>(config);
	config.renderPass = renderPass;
	config.pipelineLayout = layout;

	auto	vert = _system._assetManager.get<Shader>(_system._vertPath);
	auto	frag = _system._assetManager.get<Shader>(_system._fragPath);
	if (!vert || !frag)
		return (true);
	_pipeline.createGraphicsPipeline(config, {vert->getStageInfo(), frag->getStageInfo()});
	return (false);
}

}
