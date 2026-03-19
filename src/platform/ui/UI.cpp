/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/19 16:16:19                                        */
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

#include <fstream>

namespace	hel::sys {

UI::~UI(void) {
	saveToFile("currentLayout.json");
}

void	UI::init(void) {
	addNewPanelRegistry(EntityHierarchy::label, PanelFactoryMacro(EntityHierarchy));
	addNewPanelRegistry(StyleEditor::label, PanelFactoryMacro(StyleEditor));
	addNewPanelRegistry(Inspector::label, PanelFactoryMacro(Inspector));
	addNewPanelRegistry(SceneViewport::label, PanelFactoryMacro(SceneViewport));

	if (!loadFromFile("currentLayout.json")) {
		_dock = std::make_unique<Dock>("Dock", this);
		auto	dockChild = _dock->forceSplit(Splitter::Dir::Left, {});
		auto	leftChild = dockChild.first->forceSplit(Splitter::Dir::Left, {});

		addNewPanel<EntityHierarchy>(leftChild.first);
		addNewPanel<StyleEditor>(leftChild.first);
		addNewPanel<SceneViewport>(leftChild.second);
		addNewPanel<Inspector>(dockChild.second);
	}
}

void	UI::removePanel(IPanel *panel) {
	auto	it = std::find_if(_panels.begin(), _panels.end(),
				[panel](const auto &other){ return (panel == other.get()); });
	if (it != _panels.end())
		_panels.erase(it);
}

void	UI::saveToFile(const std::string &path) {
	std::ofstream	file(path);
	file << _dock->serialize().dump(2);
}

bool	UI::loadFromFile(const std::string &path) {
    std::ifstream	file(path);
    if (!file.is_open())	{ return (false); }
    nlohmann::json	src;
    file >> src;
	_dock = Dock::deserialize(this, src);
	return (true);
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
