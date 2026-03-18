/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/18 17:20:00                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */


#include "platform/ui/UI.hpp"
#include "platform/ui/UIHelper.hpp"
#include "ecs/Registry.hpp"
#include "api/ImGui/imgui.h"
#include "api/ImGui/imgui_internal.h"
#include "api/ImGui/imgui_stdlib.h"
#include "core/Engine.hpp"

namespace	hel::sys {

void	UI::init(void) {
	_entityHierarchy.setup(_registry, _imagePool);
	_entityHierarchy.onInit();
	_inspector.setup(_registry, _imagePool);
	_inspector.onInit();
	_styleEditor.setup(_registry, _imagePool);
	_styleEditor.onInit();
	_sceneViewport.setup(_registry, _imagePool);
	_sceneViewport.onInit();

	_entityHierarchy1 = _entityHierarchy;
	_entityHierarchy2 = _entityHierarchy;
	_entityHierarchy3 = _entityHierarchy;
	_entityHierarchy4 = _entityHierarchy;
	_entityHierarchy5 = _entityHierarchy;

	_dock = std::make_unique<Dock>("Dock");

	_inspector.setOwner(_dock.get());
	_dock->forceSplit(Splitter::Dir::Left, &_sceneViewport, {});
	auto	dockLeft = _dock->forceGetChildOne({});
	dockLeft->forceSplit(Splitter::Dir::Left, &_entityHierarchy, {});
	auto	dockLeftLeft = dockLeft->forceGetChildOne({});
	_styleEditor.setOwner(dockLeftLeft);

	_entityHierarchy1.setOwner(dockLeftLeft);
	_entityHierarchy2.setOwner(dockLeftLeft);
	_entityHierarchy3.setOwner(dockLeftLeft);
	_entityHierarchy4.setOwner(dockLeftLeft);
	_entityHierarchy5.setOwner(dockLeftLeft);
}

void	UI::addDock(Window *window, const ImVec2 &size) {
	ImGuiWindowFlags	hostFlags =
		ImGuiWindowFlags_NoTitleBar				|
		ImGuiWindowFlags_NoCollapse				|
		ImGuiWindowFlags_NoResize				|
		ImGuiWindowFlags_NoMove					|
		ImGuiWindowFlags_NoBringToFrontOnFocus	|
		ImGuiWindowFlags_NoNavFocus				|
		ImGuiWindowFlags_NoScrollbar			|
		ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::SetNextWindowPos({0.f, 0.f});
	ImGui::SetNextWindowSize(size);
	ImGui::Begin("##DockHost", nullptr, hostFlags);
	_dock->render(window, size);
	ImGui::End();

	ImGui::PopStyleVar(2);
}

void	UI::registerUI(const FrameContext &ctx) {
	auto	windowExtent = ctx.window->getExtent();
	float	windowWidth = static_cast<float>(windowExtent.width);
	float	windowHeight = static_cast<float>(windowExtent.height);

	addDock(ctx.window, {windowWidth, windowHeight});
}

}
