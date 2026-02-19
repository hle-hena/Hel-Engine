/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/19 19:26:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Device.hpp"
#include "ecs/systems/core/Camera.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "platform/window/Window.hpp"

namespace	hel::sys {

Camera::Camera(Device &device, Registry &registry, VkDescriptorSetLayout &setLayout)
	:	ISystem(device, registry, setLayout),
		_assetManager{registry.getAssetManager()} {
}

Camera::~Camera(void) {
	if (_pipelineLayout)
		vkDestroyPipelineLayout(_device.getLogical(), _pipelineLayout, nullptr);
}

Camera::SystemPipeline	*Camera::getPipelineForPass(VkFormat format, VkFormat depthFormat) {
	if (_pipelines.find(format) != _pipelines.end())
		return (_pipelines[format].get());
	auto	pipeline = std::make_unique<SystemPipeline>(*this, format, depthFormat);
	if (pipeline->init())
		return (nullptr);
	_pipelines[format] = std::move(pipeline);
	return (_pipelines[format].get());
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
}



Camera::SystemPipeline::SystemPipeline(Camera &camera)
	:	_pipeline {camera._device},
		_camera {camera} {
}

Camera::SystemPipeline::~SystemPipeline(void) {
	_pipeline.deleteGraphicsPipeline();
}

bool	Camera::SystemPipeline::init(void) {
	return (false);
}

}
