/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: main.cpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/09 17:10:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/15 17:43:41                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Engine.hpp"
#include "ecs/Entity.hpp"
#include "platform/window/Window.hpp"
#include "components/Controllers.hpp"
#include "components/Camera.hpp"
#include "components/Transform.hpp"
#include "components/Name.hpp"
#include "components/Model.hpp"
#include "GlobalData.hpp"
#include "core/RenderQueue.hpp"
#include "utils/Timer.hpp"
#include "api/vulkan/Image.hpp"

namespace	hel {

void	loadPrimaryScene(Registry *registry, Window *window) {
	Entity::id	cameraHandle = registry->createEntity();
	if (auto transform = registry->addComponent<comp::Transform>(cameraHandle).modify())
		transform->position = {0.f, -30.f, 0.f};
	registry->addComponent<comp::Controller>(cameraHandle);
	registry->addComponent<comp::EditorControllerTag>(cameraHandle);
	registry->addComponent<comp::Camera>(cameraHandle);
	registry->addComponent<comp::Name>(cameraHandle).modify()->name = "Editor Camera";
	window->setEntityReference(cameraHandle);

	Entity::id	sponzaHandle = registry->createEntity();
	if (auto mesh = registry->addComponent<comp::Model>(sponzaHandle).modify()) {
		mesh->modelName = "cube";
	}
	if (auto transform = registry->addComponent<comp::Transform>(sponzaHandle).modify()) {
		transform->position = {0.f, -80.f, 0.f};
		transform->scale = glm::vec3(0.05f);
	}
	registry->addComponent<comp::Name>(sponzaHandle).modify()->name = "Sponza";

	Entity::id	planeHandle = registry->createEntity();
	if (auto mesh = registry->addComponent<comp::Model>(planeHandle).modify()) {
		mesh->modelName = "quad";
	}
	if (auto transform = registry->addComponent<comp::Transform>(planeHandle).modify()) {
		transform->position = glm::vec3(0.f, -86.f, 0.f);
		transform->scale = glm::vec3(400.f);
	}
	registry->addComponent<comp::Name>(planeHandle).modify()->name = "Plane";

	Entity::id	secondCamera = registry->createEntity();
	if (auto transform = registry->addComponent<comp::Transform>(secondCamera).modify())
		transform->position = {40.f, 40.f, 40.f};
	registry->addComponent<comp::Controller>(secondCamera);
	registry->addComponent<comp::EditorControllerTag>(secondCamera);
	registry->addComponent<comp::Camera>(secondCamera);
	registry->addComponent<comp::Name>(secondCamera).modify()->name = "Second Camera";
}

GlobalSetBindings	defineGlobalSet(void) {
	return GlobalSetBindings()
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
					VK_SHADER_STAGE_ALL_GRAPHICS);
}

void	updateGlobalData(Registry *registry, FrameContext &ctx) {
	auto	handle = ctx.request->handle;
	auto	data = ctx.globals->get<GlobalUBO>("main UBO");
	data->viewProjection = glm::mat4{1.f};
	if (auto camera = registry->getComponent<comp::Camera>(handle)) {
		auto	extent = ctx.request->images["mainColor"]->getExtent();
		float	aspect = static_cast<float>(extent.width) /
						static_cast<float>(extent.height);
		glm::mat4 projection = glm::perspective(glm::radians(camera->fov), aspect, camera->near, camera->far);
		projection[1][1] *= -1;
		data->viewProjection = projection * camera->view;
	}
}

Timer	globalTimer;

void	tickCallback(Registry *, FrameContext &ctx) {
	{
	auto	data = ctx.globals->get<GlobalUBO>("main UBO");
	data->elapsedTime = globalTimer.elapsedTime();
	}
	{
	auto	data = ctx.globals->get<float>("delta_t");
	*data = globalTimer.lap();
	}
}

expected<void>	init(Engine &engine, EngineConfig &config) {
	auto		globalUBO = std::make_shared<GlobalUBO>();
	Ref<Buffer>	globalUBOBuffer;
	auto		delta_t = std::make_shared<float>();
	auto		globals = std::make_shared<GlobalData>();

	return engine.init(config)
		.and_then([&](void) -> expected<void>{
			globalUBO = std::make_shared<GlobalUBO>();
			auto	res = Buffer::create<GlobalUBO>(engine.device(),
						BufferConfig().dynamicAccess(true)
						.usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
						.allocFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
							VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
						.fixedCount(Frame::MAX_PASS_COUNT * Swapchain::MAX_FRAMES_IN_FLIGHT));
			if (!res)
				return unexpected(res.error());
			globalUBOBuffer = *res;
			return {};
		}).and_then([&](void) -> expected<void>{
			globals->addData("delta_t", delta_t)
				->addData("main UBO", globalUBO, globalUBOBuffer, 0);
			return engine.setUserData(globals);
		});
}

int	main(void) {
	globalTimer.start();
	Engine	engine;
	EngineConfig	config;
	config.loadPrimaryScene = loadPrimaryScene;
	config.defineGlobalSet = defineGlobalSet;
	config.updateGlobalData = updateGlobalData;
	config.tickCallback = tickCallback;
	auto	res = init(engine, config)
		.and_then([&](void)->expected<void>{ engine.run(); return {}; });
	if (!res) {
		std::cerr << res.error() << std::endl;
		return 1;
	}
	return 0;
}

}

int	main(void) {
	return hel::main();
}
