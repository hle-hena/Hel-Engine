/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Dock.cpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:31:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/18 13:30:20                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "platform/ui/Dock.hpp"
#include "api/ImGui/imgui.h"
#include "utils/mathUtils.hpp"

namespace	hel::sys {

void	Dock::split(Splitter::Dir dir, IPanel *splitPanel) {
	_type = Type::Split;
	_splitDir = (dir == Splitter::Dir::Left) ? Splitter::Dir::Right :
				(dir == Splitter::Dir::Up) ? Splitter::Dir::Down : dir;
	_splitRatio.reset();

	bool	isVertical = (_splitDir == Splitter::Dir::Right);
	_childOne = std::make_unique<Dock>(_dockName +
									(isVertical ? "_left" : "_up"));
	_childTwo = std::make_unique<Dock>(_dockName +
									(isVertical ? "_right" : "_down"));

	Dock	*target = (dir == Splitter::Dir::Left || dir == Splitter::Dir::Up)
						? _childOne.get() : _childTwo.get();
	Dock	*keep = (target == _childOne.get()) ? _childTwo.get()
												: _childOne.get();

	splitPanel->changeOwner(target);
	for (auto panel: _panels)
		panel->changeOwner(keep);
	_panels.clear();
}

void	Dock::merge(void) {
	if (_childOne->_askForMerge && _childTwo->_type == Type::Split) {
		_splitDir = _childTwo->_splitDir;
		_splitRatio = _childTwo->_splitRatio;
		_childOne = std::move(_childTwo->_childOne);
		_childTwo = std::move(_childTwo->_childTwo);
	} else if (_childTwo->_askForMerge && _childOne->_type == Type::Split) {
		_splitDir = _childOne->_splitDir;
		_splitRatio = _childOne->_splitRatio;
		_childTwo = std::move(_childOne->_childTwo);
		_childOne = std::move(_childOne->_childOne);
	} else {
		for (auto panel: _childOne->_panels)
			panel->changeOwner(this);
		for (auto panel: _childTwo->_panels)
			panel->changeOwner(this);
		_type = Type::TabGroup;
		_childOne = nullptr;
		_childTwo = nullptr;
	}
}

void	Dock::render(Window *window, const ImVec2 &size) {

	if (_type == Type::Split &&
			(_childOne->_askForMerge || _childTwo->_askForMerge))
		merge();
	if (_type == Type::Split) {
		renderSplits(window, size);
	} else {
		ImGui::BeginChild(_dockName.c_str(), size, ImGuiChildFlags_Borders);
		renderDragDrop();
		renderPanels(window, size);
		ImGui::EndChild();
	}
}

void	Dock::renderSplits(Window *window, const ImVec2 &size) {
	ImVec2	origin = ImGui::GetCursorScreenPos();
	bool	isVertical = (_splitDir == Splitter::Dir::Right);

	if (!_splitRatio)
		_splitRatio = 0.5f * (isVertical ? size.x : size.y);
	Splitter(&*_splitRatio)
		.setLabel((_dockName + "_splitter").c_str())
		.setPos(isVertical ? ImVec2(origin.x + *_splitRatio, origin.y)
						: ImVec2(origin.x, origin.y + *_splitRatio))
		.setSize(isVertical ? size.y : size.x)
		.setMin(0.f).setMax(isVertical ? size.x : size.y)
		.setDir(_splitDir)
		.build();

	auto	oneSize = isVertical ? ImVec2(*_splitRatio, size.y)
								: ImVec2(size.x, *_splitRatio);
	auto	twoSize = isVertical ? ImVec2(size.x - oneSize.x, size.y)
								: ImVec2(size.x, size.y - oneSize.y);
	ImVec2	start = ImGui::GetCursorScreenPos();

	_childOne->render(window, oneSize);
	ImGui::SetCursorScreenPos(isVertical ? ImVec2(start.x + oneSize.x, start.y)
										: ImVec2(start.x, start.y + oneSize.y));
	_childTwo->render(window, twoSize);
}

Dock::RenderDragDropContext::RenderDragDropContext(void) {
	ImVec2	avail = ImGui::GetContentRegionAvail();
	origin = ImGui::GetCursorScreenPos();
	mouse = ImGui::GetMousePos();
	tabBarH = ImGui::GetFrameHeight();

	topLeft = {origin.x, origin.y + tabBarH};
	topRight = {origin.x + avail.x, origin.y + tabBarH};
	bottomLeft = {origin.x, origin.y + avail.y};
	bottomRight = {origin.x + avail.x, origin.y + avail.y};
	center = {origin.x + avail.x * 0.5f,
		origin.y + tabBarH + (avail.y - tabBarH) * 0.5f};

	released = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
}

bool	Dock::renderTriangleZones(const RenderDragDropContext &ctx,
								ImDrawList *draw, IPanel *panel) {
	struct	Zone { ImVec2	a, b, c;	Splitter::Dir	dir; };
	Zone	zones[] = {
		{ctx.topLeft, ctx.topRight, ctx.center, Splitter::Dir::Up},
		{ctx.topRight, ctx.bottomRight, ctx.center, Splitter::Dir::Right},
		{ctx.bottomRight, ctx.bottomLeft, ctx.center, Splitter::Dir::Down},
		{ctx.bottomLeft, ctx.topLeft, ctx.center, Splitter::Dir::Left}
	};
	for (auto &zone: zones) {
		if (!(mathUtils::pointInTriangle(ctx.mouse, zone.a, zone.b, zone.c)))
			continue ;
		draw->AddTriangleFilled(zone.a, zone.b, zone.c, IM_COL32(255, 0, 0, 50));
		if (ctx.released)	{ split(zone.dir, panel); }
		return (true);
	}
	return (false);
}

void	Dock::renderTabBarZone(const RenderDragDropContext &ctx,
							ImDrawList *draw, IPanel *panel) {
	auto	&rectMin = ctx.origin;
	auto	&rectMax = ctx.topRight;
	if (ctx.mouse.x >= rectMin.x && ctx.mouse.x <= rectMax.x &&
		ctx.mouse.y >= rectMin.y && ctx.mouse.y <= rectMax.y) {
		draw->AddRectFilled(rectMin, rectMax, IM_COL32(255, 0, 0, 50));
		if (ctx.released)	{ panel->changeOwner(this); }
	}
}

void	Dock::renderDragDrop(void) {
	auto	*payload = ImGui::GetDragDropPayload();
	if (!payload || !payload->IsDataType("TAB_MOVE"))
		return ;

	RenderDragDropContext	ctx;
	IPanel	*panel = *static_cast<IPanel**>(payload->Data);
	auto	draw = ImGui::GetForegroundDrawList();

	if (renderTriangleZones(ctx, draw, panel))
		return ;
	renderTabBarZone(ctx, draw, panel);
}

void	Dock::renderPanels(Window *window, const ImVec2 &size) {
	DropTarget("TAB_MOVE")
		.build([this](const ImGuiPayload *payload){
			IPanel* panel = *static_cast<IPanel**>(payload->Data);
			panel->changeOwner(this);
		});

	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_NoTabListScrollingButtons)) {
		ImVec2	effectiveSize = ImGui::GetContentRegionAvail();
		for (auto panel: _panels) {
			bool	open = ImGui::BeginTabItem(panel->getLabel());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				ImGui::SetDragDropPayload("TAB_MOVE", &panel, sizeof(IPanel *));
				ImGui::Text(panel->getLabel());
				ImGui::EndDragDropSource();
			}
			if (open) {
				panel->render(window, effectiveSize);
				ImGui::EndTabItem();
			}
		}
		if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing)) {
			std::cout << "Open a new tab\n";
		}
		ImGui::EndTabBar();
	}
	if (_panels.empty() && !ImGui::GetDragDropPayload())
		_askForMerge = true;
}

}
