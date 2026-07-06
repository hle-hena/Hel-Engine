/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Camera.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/06 10:52:39                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "systems/Camera.hpp"
#include "components/Transform.hpp"
#include "components/Camera.hpp"
#include "components/HideTag.hpp"
#include "assetType/Geometry.hpp"
#include "assetType/Texture.hpp"

namespace	hel::sys {

SystemRegistrar<Camera>	reg_CameraSystem;

void	Camera::init(void) {
	addUpdateDep("view matrix calculation", &Camera::update)
	->getDep()->addRequire("model matrix calculation");

	addRenderDep("render camera frustum", &Camera::renderInteraction)
	->getDep()
		->addBlock("render transform gizmo")
		->addActiveLayer("RenderScene")
		->startWrite<Color>("mainColor", VK_FORMAT_B8G8R8A8_SRGB, 0)
			.addDep()
		->startWrite<Color>("entity layer", VK_FORMAT_R32_UINT, 1)
			.addDep()
		->startWrite<DepthStencil>("gizmo depth layer", VK_FORMAT_D32_SFLOAT_S8_UINT)
			.config(Image::Config()
				.setFormats(VK_FORMAT_D32_SFLOAT_S8_UINT)
				.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
				.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT
						| VK_IMAGE_ASPECT_STENCIL_BIT))
			.addDep();

	_assetManager = _registry->assetManager();
	{
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = _assetManager;
		config.shaderPaths = {
			"cameraFrustum.vert",
			"cameraFrustum.frag"
		};
		config.initPipelineLayout = initFrustumLayout;
		config.configurePipeline = configureFrustumPipeline;
		_frustumPipeline = createPipeline(config);
	}
	{
		PipelineMap::Config	config;
		config.device = _device;
		config.assetManager = _assetManager;
		config.shaderPaths = {
			"billboard.vert",
			"billboard.frag"
		};
		config.initPipelineLayout = initSpriteLayout;
		config.configurePipeline = configureSpritePipeline;
		_spritePipeline = createPipeline(config);
	}
}

void	Camera::initFrustumLayout(Device &, std::vector<VkDescriptorSetLayout> &,
								std::vector<VkPushConstantRange> &pushConstant) {
	VkPushConstantRange	vertexPush{};
	vertexPush.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vertexPush.size = sizeof(FrustumPush);
	pushConstant.push_back(vertexPush);
}

void	Camera::configureFrustumPipeline(PipelineConfig &config) {
	Pipeline::setVertexInputDescriptions<Vertex>(config);
	config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
}

void	Camera::initSpriteLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
								std::vector<VkPushConstantRange> &pushConstant) {
	VkPushConstantRange	push{};
	push.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	push.size = sizeof(EntityData);
	pushConstant.push_back(push);

	setLayouts.push_back(DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL)
							.getSetLayout());
	auto	set = DescriptorFactory(device)
							.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
								VK_SHADER_STAGE_FRAGMENT_BIT, Sampler::getSampler(device, {}), 1)
							.getSetLayout();
	setLayouts.push_back(set);
}

void	Camera::configureSpritePipeline(PipelineConfig &config) {
	config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

	VkPipelineColorBlendAttachmentState	attachment{};
	attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
								VK_COLOR_COMPONENT_G_BIT |
								VK_COLOR_COMPONENT_B_BIT |
								VK_COLOR_COMPONENT_A_BIT;
	attachment.blendEnable = VK_FALSE;
	config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	Pipeline::setBlendAttachment(config, 1, attachment);
}

void	Camera::update(const FrameContext &) {
	auto	entities = _registry->view<include<comp::Transform, comp::Camera>>();

	for (auto entity: entities) {
		auto	constTransform = entities.get<comp::Transform>(entity);
		auto	constCamera = entities.get<comp::Camera>(entity);

		if (!constCamera.isDirty() && !constTransform.isDirty())
			continue ;
		if (auto camera = constCamera.modify()) {
			glm::mat4 rotate = glm::mat4_cast(glm::conjugate(constTransform->rotation));
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), -constTransform->position);
			glm::mat4 view = rotate * translate;

			camera->view = view;
		}
	}
}

void	Camera::renderInteraction(const Renderer &renderer) {
	auto	&ctx = renderer.frameContext();
	auto	selfHandle = ctx.request->handle;
	auto	selfCam = _registry->getComponent<comp::Camera>(selfHandle);
	auto	selfTransform = _registry->getComponent<comp::Transform>(selfHandle);
	if (!selfCam || !selfTransform)
		return ;
	auto	commandBuffer = ctx.commandBuffer;
	if (!commandBuffer)	{ return ; }

	auto	entities = _registry->view<
				include<comp::Camera, comp::Transform>,
				exclude<comp::HideEntityTag>>();
	auto	sampler = Sampler::getSampler(*_device, {});
	auto	texture_d = DescriptorFactory(*_device)
						.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
							VK_SHADER_STAGE_FRAGMENT_BIT, sampler, 1)
						.build(*ctx.descriptorPool);
	auto	texture = _assetManager->get<Texture>("assets/images/cameraSprite.png");
	DescriptorWriter(*_device, texture_d.get())
		.writeImage(0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				texture->image->getView(ViewConfig().defaultTextureView()),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, sampler)
		.update();
	auto	SSBO_d = _registry->buildComponentSet<comp::Transform>(*_device, ctx.descriptorPool);
	if (!SSBO_d)
		return ;
	for (auto entity : entities) {
		if (entity == selfHandle)	{ continue ; }
		auto	mesh = _assetManager->get<FullGeometry>("frustum");
		if (!mesh)	{ continue ; }
		auto	transform = entities.get<comp::Transform>(entity);
		auto	camera = entities.get<comp::Camera>(entity);

		glm::mat4 projection = glm::perspective(glm::radians(camera->fov), 1.f, camera->near, camera->far);
		projection[1][1] *= -1;
		drawCommand(renderer, _frustumPipeline)
			.addPush(VK_SHADER_STAGE_VERTEX_BIT,
					FrustumPush{transform->worldMatrix,
					glm::inverse(projection * camera->view)})
			.addVertexBuffers({mesh->vertexBuffer->getBuffer()}, {0})
			.addIndexBuffer(mesh->lineIndexBuffer->getBuffer())
			.setVertexCount(mesh->lineVertexCount)
			.submit();

		float	size = 0.1f * glm::distance(transform->position, selfTransform->position);
		drawCommand(renderer, _spritePipeline)
			.addBinding(SSBO_d->sets[0])
			.addBinding(texture_d->sets[0])
			.addPush(VK_SHADER_STAGE_ALL_GRAPHICS, EntityData{entity,
					transform.getDenseIndex(), size})
			.setVertexCount(4)
			.submit();
	}
}

}
