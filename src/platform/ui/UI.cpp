/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UI.cpp                                                              */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/27 11:06:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/10 16:56:59                                        */
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

UI::UI(Device &device, Registry &registry)
	:	ISystem(device, registry),
		_inspectorUI{registry},
		_entityHierarchyUI{registry} {
	_inspectorUI.setBuiltInDrawFunc();
}

UI::~UI(void) {
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

void	UI::render(const RenderingConfig &, WindowResources &resources,
				uint32_t) {
	auto	windowExtent = resources.window->getExtent();
	float	windowWidth = static_cast<float>(windowExtent.width);
	float	windowHeight = static_cast<float>(windowExtent.height);

	_inspectorUI.render(resources.window, {windowWidth - _rightTabWidth, 0.f},
						{_rightTabWidth, windowHeight});
	_entityHierarchyUI.render(resources.window, {0.f, 0.f},
							{_leftTabWidth, windowHeight});
	_sceneViewport.render(resources.window, {_leftTabWidth, 0.f},
						{windowWidth - _rightTabWidth - _leftTabWidth, windowHeight});

	addSplitters(windowWidth, windowHeight);
}

}
