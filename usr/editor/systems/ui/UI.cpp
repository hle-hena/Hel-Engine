/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/08 15:05:42                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */


#include "systems/ui/UI.hpp"
#include "systems/ui/UIHelper.hpp"
#include "systems/ui/EntityHierarchy.hpp"
#include "systems/ui/StyleEditor.hpp"
#include "systems/ui/SceneViewport.hpp"
#include "systems/ui/Inspector.hpp"
#include "api/vulkan/Renderer.hpp"
#include "platform/ui/UiContext.hpp"
#include "systems/ui/Dock.hpp"
#include "core/SystemManager.hpp"
#include "platform/window/Window.hpp"

#include <ui/ImGui/imgui.h>
#include <fstream>

namespace	hel::sys {

SystemRegistrar<UI>	reg_UISystem;

UI::~UI(void) {
	saveToFile("currentLayout.json");
	StyleEditor::saveToFile("currentStyle.json");
}

void	UI::init(void) {
	addUpdateDep("input/ui", &UI::updateUI);

	addRenderDep("render/ui", &UI::render)
	->getDep()
		->addActiveLayer("RenderUI")
		->startWrite<Color>("mainColor", VK_FORMAT_B8G8R8A8_UNORM, 0)
			.addDep()
		->addRead("viewport*")
		->addRead("depth*")
		->addRead("entity*");

	addNewPanelRegistry(EntityHierarchy::label, PanelFactoryMacro(EntityHierarchy));
	addNewPanelRegistry(StyleEditor::label, PanelFactoryMacro(StyleEditor));
	addNewPanelRegistry(Inspector::label, PanelFactoryMacro(Inspector));
	addNewPanelRegistry(SceneViewport::label, PanelFactoryMacro(SceneViewport));

	if (!loadFromFile("currentLayout.json")) {
		_dock = std::make_unique<Dock>("Dock", this);
		auto	dockChild = _dock->forceSplit(Splitter::Dir::Left, -0.835f, {});
		auto	leftChild = dockChild.first->forceSplit(Splitter::Dir::Left, -0.139f, {});

		addNewPanel<EntityHierarchy>(leftChild.first);
		addNewPanel<SceneViewport>(leftChild.second);
		addNewPanel<Inspector>(dockChild.second);
	}

	sys::StyleEditor::loadFromFile("currentStyle.json");
	sys::StyleEditor::applyPalette();
}

void	UI::removePanel(IPanel *panel) {
	auto	it = std::find_if(_panels.begin(), _panels.end(),
				[panel](const auto &other){ return (panel == other.get()); });
	if (it != _panels.end())
		_panels.erase(it);
}

void	UI::saveToFile(const std::string &path) {
	if (_dock) {
		std::ofstream	file(path);
		file << _dock->serialize(*_lastSize).dump(2);
	}
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
	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetCursorScreenPos(),
		ImGui::GetContentRegionAvail(),
		ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg))
	);
	if (!_lastSize)
		_lastSize = size;
	if (size.x > 0.f && size.y > 0.f &&
			((*_lastSize).x != size.x || (*_lastSize).y != size.y)) {
		_dock->render(&_request, window, size, {size.x / (*_lastSize).x, size.y / (*_lastSize).y});
		_lastSize = size;
	}
	else
		_dock->render(&_request, window, size);
	ImGui::End();

	ImGui::PopStyleVar(2);
}

void	UI::updateUI(const FrameContext &ctx) {
	auto	windowExtent = ctx.window->getExtent();
	float	windowWidth = static_cast<float>(windowExtent.width);
	float	windowHeight = static_cast<float>(windowExtent.height);

	_request.requestType = "RenderUI";
	_request.handle = Entity::NOT_REGISTERED;
	_request.origin = {0, 0};
	_request.images = {{"mainColor", ctx.window->getSwapchain()
									.getSwapImage(ctx.swapIndex)}};
	addDock(ctx.window, {windowWidth, windowHeight});
	RenderQueue::push(_request);
}

void	UI::render(const Renderer &renderer) {
	auto	ctx = renderer.frameContext();
	UiContext::renderFrame(ctx.commandBuffer);
}

}
