/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/17 19:57:16                                        */
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

	_leftDock = std::unique_ptr<Dock>(new Dock("Left dock"));
	_rightDock = std::unique_ptr<Dock>(new Dock("Right dock"));
}

void	UI::addSplitters(float windowWidth, float windowHeight) {
	Splitter(&_leftTabWidth)
		.setLabel("leftTab splitter")
		.setMin(50.f)
		.setMax(windowWidth * 0.35f)
		.setPos({_leftTabWidth, 0.f})
		.setSize(windowHeight)
		.setDir(Splitter::Right)
		.build();

	Splitter(&_rightTabWidth)
		.setLabel("rightTab splitter")
		.setMin(50.f)
		.setMax(windowWidth * 0.4f)
		.setPos({windowWidth - _rightTabWidth, 0.f})
		.setSize(windowHeight)
		.setDir(Splitter::Left)
		.build();
}

void	UI::addDockSpaces(Window *window, float windowWidth, float windowHeight) {
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
	ImGui::SetNextWindowSize({_leftTabWidth, windowHeight});
	ImGui::Begin("##LeftDockHost", nullptr, hostFlags);
	_leftDock->render(window, {_leftTabWidth, windowHeight});
	ImGui::End();

	ImGui::SetNextWindowPos({windowWidth - _rightTabWidth, 0.f});
	ImGui::SetNextWindowSize({_rightTabWidth, windowHeight});
	ImGui::Begin("##RightDockHost", nullptr, hostFlags);
	_rightDock->render(window, {_rightTabWidth, windowHeight});
	ImGui::End();

	ImGui::PopStyleVar(2);
}

void	UI::initDockLayout(void) {
	static bool	initialized = false;
	if (initialized)	{ return ; }
	initialized = true;

	_entityHierarchy.setOwner(_leftDock.get());
	_styleEditor.setOwner(_leftDock.get());
	_inspector.setOwner(_rightDock.get());
}

void	UI::registerUI(const FrameContext &ctx) {
	auto	windowExtent = ctx.window->getExtent();
	float	windowWidth = static_cast<float>(windowExtent.width);
	float	windowHeight = static_cast<float>(windowExtent.height);

	addSplitters(windowWidth, windowHeight);
	initDockLayout();
	addDockSpaces(ctx.window, windowWidth, windowHeight);

	_sceneViewport.render(_imagePool, ctx.window, {_leftTabWidth, 0.f},
						{std::max(windowWidth - _rightTabWidth - _leftTabWidth, 1.f), std::max(windowHeight, 1.f)});
}

}
