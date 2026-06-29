/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: main.cpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/09 17:10:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/28 10:05:26                                        */
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
#include "ecs/Component.hpp"
#include "GlobalData.hpp"
#include "core/Queues.hpp"

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
		mesh->modelName = "sponza";
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
	auto	data = static_cast<GlobalUBO *>((*ctx.globalData)[0].data);
	data->viewProjection = glm::mat4{1.f};
	if (auto camera = registry->getComponent<comp::Camera>(handle)) {
		auto	extent = ctx.request->images["mainColor"]->getExtent();
		float	aspect = static_cast<float>(extent.width) /
						static_cast<float>(extent.height);
		glm::mat4 projection = glm::perspective(glm::radians(camera->fov), aspect, camera->near, camera->far);
		projection[1][1] *= -1;
		// ctx.projection = projection;
		data->viewProjection = projection * camera->view;
	}
	// ctx.globalData.elapsedTime = _timer.elapsedTime();
}

int	main(void) {
	Engine	engine;
	EngineConfig	config;
	config.loadPrimaryScene = &loadPrimaryScene;
	config.defineGlobalSet = defineGlobalSet;
	config.updateGlobalData = updateGlobalData;

	auto	res = engine.init(config);
	if (!res) {
		std::cerr << res.error() << std::endl;
		return 1;
	}

	GlobalUBO globalUBO;

	std::vector<UserData>	data;
	data.push_back(UserData{.data = &globalUBO, .buffer = nullptr, .bindingIndex = 0});
	res = engine.setUserData(&data);
	if (!res) {
		std::cerr << res.error() << std::endl;
		return 1;
	}

	engine.run();

	return 0;
}

}

int	main(void) {
	return hel::main();
}
