/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 19:37:03                                        */
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
#include "api/ImGui/imgui_stdlib.h"
#include "core/Engine.hpp"

namespace	hel::sys {

void	UI::init(void) {
	_inspectorUI.init(_registry);
	_inspectorUI.setBuiltInDrawFunc();
	_entityHierarchyUI.init(_registry);
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

void	UI::render(const FrameContext &ctx, const RenderingConfig &conf) {
	auto	windowExtent = ctx.window->getExtent();
	float	windowWidth = static_cast<float>(windowExtent.width);
	float	windowHeight = static_cast<float>(windowExtent.height);

	addSplitters(windowWidth, windowHeight);

	_inspectorUI.render(ctx.window, {std::max(windowWidth - _rightTabWidth, 1.f), 0.f},
						{_rightTabWidth, std::max(windowHeight, 1.f)});
	_entityHierarchyUI.render(ctx.window, {0.f, 0.f},
							{_leftTabWidth, std::max(windowHeight, 1.f)});
	_sceneViewport.render(_imagePool, ctx.window, {_leftTabWidth, 0.f},
						{std::max(windowWidth - _rightTabWidth - _leftTabWidth, 1.f), std::max(windowHeight, 1.f)});
}

}
