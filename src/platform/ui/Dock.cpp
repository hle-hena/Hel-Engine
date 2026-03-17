/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Dock.cpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:31:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/17 21:29:44                                        */
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

	if (dir == Splitter::Dir::Left || dir == Splitter::Dir::Right) {
		_childOne = std::unique_ptr<Dock>(new Dock(_dockName + "_leftChild"));
		_childTwo = std::unique_ptr<Dock>(new Dock(_dockName + "_rightChild"));
	} else {
		_childOne = std::unique_ptr<Dock>(new Dock(_dockName + "_upChild"));
		_childTwo = std::unique_ptr<Dock>(new Dock(_dockName + "_downChild"));
	}

	Dock	*target = nullptr;
	Dock	*keep = nullptr;
	if (dir == Splitter::Dir::Left || dir == Splitter::Dir::Up) {
		target = _childOne.get();
		keep = _childTwo.get();
	} else {
		target = _childTwo.get();
		keep = _childOne.get();
	}

	splitPanel->changeOwner(target);
	for (auto panel: _panels)
		panel->changeOwner(keep);
	_panels.clear();

	_splitDir = dir;
	if (_splitDir == Splitter::Dir::Left)
		_splitDir = Splitter::Dir::Right;
}

void	Dock::merge(void) {
	
}

void	Dock::render(Window *window, ImVec2 size) {
	if (_type == Type::Split) {
		ImVec2	origin = ImGui::GetCursorScreenPos();
		bool	splitVertical = (_splitDir == Splitter::Dir::Left ||
								_splitDir == Splitter::Dir::Right);

		ImVec2	pos = splitVertical ?
						ImVec2(origin.x + size.x * _splitRatio, origin.y) :
						ImVec2(origin.x, origin.y + size.y * _splitRatio);
		float	splitterSize = splitVertical ? size.y : (size.x);
		Splitter(&_splitRatio)
			.setLabel((_dockName + "_splitter").c_str())
			.setDir(_splitDir)
			.setMin(0.f)
			.setMax(1.f)
			.setPos(pos)
			.setNormalizer(splitVertical ? size.x : size.y)
			.setSize(splitterSize)
			.build();

		auto	childOneSize = splitVertical ?
									ImVec2(size.x * _splitRatio, size.y) :
									ImVec2(size.x, size.y * _splitRatio);
		auto	childTwoSize = splitVertical ?
									ImVec2(size.x - childOneSize.x, size.y) :
									ImVec2(size.x, size.y - childOneSize.y);
		ImVec2	startPos = ImGui::GetCursorScreenPos();

		_childOne->render(window, childOneSize);
		if (splitVertical)
			ImGui::SetCursorScreenPos({startPos.x + childOneSize.x, startPos.y});
		else
			ImGui::SetCursorScreenPos({startPos.x, startPos.y + childOneSize.y});
		_childTwo->render(window, childTwoSize);
	} else {
		ImGui::BeginChild(_dockName.c_str(), size, ImGuiChildFlags_Borders);
		renderDragDrop();
		renderPanels(window);
		ImGui::EndChild();
	}
}

void	Dock::renderDragDrop(void) {
	if (auto payload = ImGui::GetDragDropPayload()) {
		if (!payload->IsDataType("TAB_MOVE"))
			return ;
	}
	else
		return ;

	ImVec2	avail = ImGui::GetContentRegionAvail();
	ImVec2	origin = ImGui::GetCursorScreenPos();
	float	tabBarH = ImGui::GetFrameHeight();

	ImVec2	topLeft = {origin.x, origin.y + tabBarH};
	ImVec2	topRight = {origin.x + avail.x, origin.y + tabBarH};
	ImVec2	bottomLeft = {origin.x, origin.y + avail.y};
	ImVec2	bottomRight = {origin.x + avail.x, origin.y + avail.y};
	ImVec2	center = {origin.x + avail.x * 0.5f,
		origin.y + tabBarH + (avail.y - tabBarH) * 0.5f};

	struct	Zone {
		ImVec2			a, b, c;
		Splitter::Dir	dir;
	};

	Zone	zones[] = {
		{topLeft, topRight, center, Splitter::Dir::Up},
		{topRight, bottomRight, center, Splitter::Dir::Right},
		{bottomRight, bottomLeft, center, Splitter::Dir::Down},
		{bottomLeft, topLeft, center, Splitter::Dir::Left}
	};

	ImVec2	mousePos = ImGui::GetMousePos();
	auto	draw = ImGui::GetForegroundDrawList();

	for (auto &zone: zones) {
		if (!mathUtils::pointInTriangle(mousePos, zone.a, zone.b, zone.c))
			continue ;
		draw->AddTriangleFilled(zone.a, zone.b, zone.c, IM_COL32(255, 0, 0, 50));
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			if (auto payload = ImGui::GetDragDropPayload()) {
				IPanel* panel = *static_cast<IPanel**>(payload->Data);
				split(zone.dir, panel);
			}
		}
	}
	bottomLeft = topLeft;
	bottomRight = topRight;
	topLeft = origin;
	topRight = {origin.x + avail.x, origin.y};
	if (mathUtils::pointInTriangle(mousePos, topLeft, bottomRight, bottomLeft) ||
		mathUtils::pointInTriangle(mousePos, topLeft, topRight, bottomRight)) {
		draw->AddRectFilled(topLeft, bottomRight, IM_COL32(255, 0, 0, 50));
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			if (auto payload = ImGui::GetDragDropPayload()) {
				IPanel* panel = *static_cast<IPanel**>(payload->Data);
				panel->changeOwner(this);
			}
		}
	}
}

void	Dock::renderPanels(Window *window) {
	DropTarget("TAB_MOVE")
		.build([this](const ImGuiPayload *payload){
			IPanel* panel = *static_cast<IPanel**>(payload->Data);
			panel->changeOwner(this);
		});
	
	if (ImGui::BeginTabBar("##tabs")) {
		for (auto panel: _panels) {
			bool	open = ImGui::BeginTabItem(panel->getLabel());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				ImGui::SetDragDropPayload("TAB_MOVE", &panel, sizeof(IPanel *));
				ImGui::Text(panel->getLabel());
				ImGui::EndDragDropSource();
			}
			if (open) {
				panel->render(window);
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

}
