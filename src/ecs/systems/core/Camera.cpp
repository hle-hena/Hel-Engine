/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/19 17:22:28                                        */
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

Camera::SystemPipeline	*Camera::getPipelineForFormat(VkFormat format, VkFormat depthFormat) {
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

void	Camera::render(WindowResources &resources, uint32_t currentFrame,
					uint32_t imageIndex) {
}



Camera::SystemPipeline::SystemPipeline(Camera &camera, VkFormat format, VkFormat depthFormat)
	:	_format {format},
		_depthFormat {depthFormat},
		_pipeline {camera._device},
		_camera {camera} {
}

Camera::SystemPipeline::~SystemPipeline(void) {
	_pipeline.deleteGraphicsPipeline();
}

bool	Camera::SystemPipeline::init(void) {
	return (createRenderPass() || createPipeline());
}

bool	Camera::SystemPipeline::createRenderPass(void) {
	VkAttachmentDescription	colorAttachment{};
	colorAttachment.format = _format;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkAttachmentDescription	depthAttachment{};
	colorAttachment.format = _depthFormat;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkRenderPassCreateInfo	createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;


	if (vkCreateRenderPass(_camera._device.getLogical(), &createInfo, nullptr,
							&_renderPass))
		return (true);
	return (false);
}

bool	Camera::SystemPipeline::createPipeline(void) {
	return (false);
}

}
