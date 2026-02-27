/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Application.cpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 14:49:32 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/27 17:52:47                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "core/Application.hpp"
#include "utils/healthHelper.hpp"
#include "platform/window/GLFW.hpp"
#include "ecs/Component.hpp"

#include <iostream>

namespace hel {

Application::Application(void)
	:	_appWindows{},
		_vkContext{*this},
		_registry{_assetManager},
		_assetManager{_vkContext.getDevice()},
		_engine{_vkContext.getDevice(), _registry} {
	if (!GLFW::acquire())
		RETURN_SET_UNHEALTHY("Couldn't init glfw.");
	if (_vkContext.initiateVulkan()) {
		GLFW::release();
		RETURN_SET_UNHEALTHY(_vkContext.getReason());
	}
	addNewWindow(Window::WIDTH, Window::HEIGHT, "Hel");
	GLFW::release();
	if (_appWindows.size() == 0)
		RETURN_SET_UNHEALTHY("Couldn't even create one window");
	if (_engine.init(*_appWindows.back()))
		RETURN_SET_UNHEALTHY(_engine.getReason());
	loadPrimaryScene();
}

Application::~Application(void) {
}

void	Application::loadPrimaryScene(void) {
	Entity::id	dragonHandle = _registry.createEntity();
	if (auto mesh = _registry.modify(_registry.addComponent<comp::Model>(dragonHandle))) {
		mesh->filePath = "assets/models/dragon.obj";
	}
	if (auto transform = _registry.modify(_registry.addComponent<comp::Transform>(dragonHandle))) {
		transform->scale = glm::vec3(4.f);
	}
	Entity::id	dragonSecondHandle = _registry.createEntity();
	if (auto mesh = _registry.modify(_registry.addComponent<comp::Model>(dragonSecondHandle))) {
		mesh->filePath = "assets/models/dragon.obj";
	}
	if (auto transform = _registry.modify(_registry.addComponent<comp::Transform>(dragonSecondHandle))) {
		transform->position = glm::vec3(2.f, 0.f, 2.f);
		transform->rotation = glm::quat(0.932, -0.267, 0.237, -0.059);
	}
	Entity::id	handle = _registry.createEntity();
	if (auto mesh = _registry.modify(_registry.addComponent<comp::Model>(handle))) {
		mesh->filePath = "assets/models/flat_vase.obj";
	}
	if (auto transform = _registry.modify(_registry.addComponent<comp::Transform>(handle))) {
		transform->position = glm::vec3(-2.f, 0.f, -2.f);
		transform->scale = glm::vec3(4.f, 2.f, 4.f);
		transform->scale.y = -transform->scale.y;
	}
	Entity::id	secondHandle = _registry.createEntity();
	if (auto mesh = _registry.modify(_registry.addComponent<comp::Model>(secondHandle))) {
		mesh->filePath = "assets/models/smooth_vase.obj";
	}
	if (auto transform = _registry.modify(_registry.addComponent<comp::Transform>(secondHandle))) {
		transform->position = glm::vec3(2.f, 0.f, 2.f);
		transform->scale = glm::vec3(4.f);
		transform->scale.y = -transform->scale.y;
	}
	Entity::id	thirdHandle = _registry.createEntity();
	if (auto mesh = _registry.modify(_registry.addComponent<comp::Model>(thirdHandle))) {
		mesh->filePath = "assets/models/quad.obj";
	}
	if (auto transform = _registry.modify(_registry.addComponent<comp::Transform>(thirdHandle))) {
		transform->position = glm::vec3(0.f, 0.f, 0.f);
		transform->scale = glm::vec3(400.f);
	}
}

void	Application::run(void) {
	uint32_t	currentFrame = 0;

	while (!_appWindows.empty() && _healthy) {
		_registry.getInputState().newFrame();
		glfwPollEvents();

		_engine.updateFrame();
		for (size_t i = 0; i < _appWindows.size(); i++) {
			if (_appWindows[i]->shouldClose()) {
				_appWindows.erase(_appWindows.begin() + i);
				i--;
				continue ;
			}
			_engine.renderFrame(*_appWindows[i], currentFrame);
		}

		currentFrame = (currentFrame + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
		_registry.resetAllDirty();
	}
	vkDeviceWaitIdle(_vkContext.getDevice().getLogical());
	if (!_healthy)
		std::cerr << _reason << std::endl;
}

void	Application::addNewWindow(int width, int height, const std::string &windowName) {
	Window::windowPtr window = Window::createWindow(width, height, windowName, *this,
												_vkContext.getInstance().getVkInstance());
	if (!window) {
		std::cerr << "Failed to create a new window." << std::endl;
		return ;
	}
	if (!_vkContext.getDevice().supportSurface(*window.get())) {
		std::cerr << "The window surface is not supported." << std::endl;
		return ;
	}
	Entity::id	handle = _registry.createEntity();
	auto	transform = _registry.modify(_registry.addComponent<comp::Transform>(handle));
	transform->position = {1.f, 1.f, 1.f};
	_registry.addComponent<comp::Controller>(handle);
	if (windowName == "Hel") {
		_registry.addComponent<comp::EditorControllerTag>(handle);
	} else {
		_registry.addComponent<comp::BaseControllerTag>(handle);
		auto allign = _registry.modify(_registry.addComponent<comp::SurfaceAllignement>(handle));
		allign->isDynamic = true;
	}
	auto camera = _registry.modify(_registry.addComponent<comp::Camera>(handle));
	camera->aspect = static_cast<float>(width) / static_cast<float>(height);
	window->setEntityReference(handle);
	_appWindows.push_back(std::move(window));
}

}
