/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Dock.cpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:31:03 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/21 13:04:26                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "systems/ui/Dock.hpp"
#include "systems/ui/UI.hpp"
#include "utils/mathUtils.hpp"
#include "systems/ui/SceneViewport.hpp"

#include <ui/ImGui/imgui.h>
#include <ui/ImGui/imgui_internal.h>

namespace	hel::sys {

nlohmann::json	Dock::serialize(const ImVec2 &size) const {
	nlohmann::json	dst;
	dst["name"] = _dockName;
	if (_type == Type::Split) {
		dst["type"] = "Split";
		dst["splitDir"] = static_cast<int>(_splitDir);
		bool	isVertical = (_splitDir == Splitter::Dir::Right);
		dst["splitRatio"] = (*_splitRatio) / (isVertical ? size.x : size.y);
		dst["childOne"] = _childOne->serialize(isVertical ?
								ImVec2(*_splitRatio, size.y) :
								ImVec2(size.x, *_splitRatio));
		dst["childTwo"] = _childTwo->serialize(isVertical ?
								ImVec2(size.x - *_splitRatio, size.y) :
								ImVec2(size.y, size.y - *_splitRatio));
	} else {
		dst["type"] = "TabGroup";
		dst["panels"] = nlohmann::json::array();
		for (auto panel: _panels)
			dst["panels"].push_back(panel->getLabel());
	}
	return (dst);
}

std::unique_ptr<Dock>	Dock::deserialize(UI *ui, const nlohmann::json &src) {
	auto	dock = std::make_unique<Dock>(src["name"], ui);
	if (src["type"] == "Split") {
		dock->_type = Type::Split;
		dock->_splitDir = static_cast<Splitter::Dir>(src["splitDir"].get<int>());
		dock->_splitRatio = -src["splitRatio"].get<float>();
		dock->_childOne = Dock::deserialize(ui, src["childOne"]);
		dock->_childTwo = Dock::deserialize(ui, src["childTwo"]);
	} else {
		dock->_type = Type::TabGroup;
		for (auto &panelLabel: src["panels"]) {
			auto	&panelRegistry = ui->getPanelRegistry();
			auto	it = std::find_if(panelRegistry.begin(), panelRegistry.end(),
					[&](const auto &r){ return (r.first == panelLabel.get<std::string>()); });
			if (it != panelRegistry.end())
				it->second(ui, dock.get());
		}
	}
	return (dock);
}

std::pair<Dock *, Dock *>	Dock::forceSplit(Splitter::Dir dir,
											float splitRatio, UIKey) {
	_type = Type::Split;
	_splitDir = (dir == Splitter::Dir::Left) ? Splitter::Dir::Right :
				(dir == Splitter::Dir::Up) ? Splitter::Dir::Down : dir;
	_splitRatio = splitRatio;

	bool	isVertical = (_splitDir == Splitter::Dir::Right);
	_childOne = std::make_unique<Dock>(_dockName +
									(isVertical ? "_left" : "_up"), _ui);
	_childTwo = std::make_unique<Dock>(_dockName +
									(isVertical ? "_right" : "_down"), _ui);

	Dock	*keep = (dir == Splitter::Dir::Left || dir == Splitter::Dir::Up)
						? _childTwo.get() : _childTwo.get();

	auto	panels = std::move(_panels);
	for (auto panel: panels)
		panel->changeOwner(keep);
	_panels.clear();
	return {_childOne.get(), _childTwo.get()};
}

void	Dock::split(Splitter::Dir dir, IPanel *splitPanel) {
	_type = Type::Split;
	_splitDir = (dir == Splitter::Dir::Left) ? Splitter::Dir::Right :
				(dir == Splitter::Dir::Up) ? Splitter::Dir::Down : dir;
	_splitRatio.reset();

	bool	isVertical = (_splitDir == Splitter::Dir::Right);
	_childOne = std::make_unique<Dock>(_dockName +
									(isVertical ? "_left" : "_up"), _ui);
	_childTwo = std::make_unique<Dock>(_dockName +
									(isVertical ? "_right" : "_down"), _ui);

	Dock	*target = (dir == Splitter::Dir::Left || dir == Splitter::Dir::Up)
						? _childOne.get() : _childTwo.get();
	Dock	*keep = (target == _childOne.get()) ? _childTwo.get()
												: _childOne.get();

	splitPanel->changeOwner(target);
	auto	panels = std::move(_panels);
	for (auto panel: panels)
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
		auto	panels = std::move(_childOne->_panels);
		for (auto panel: panels)
			panel->changeOwner(this);
		panels = std::move(_childTwo->_panels);
		for (auto panel: panels)
			panel->changeOwner(this);
		_type = Type::TabGroup;
		_childOne = nullptr;
		_childTwo = nullptr;
	}
}

void	Dock::render(RenderRequest *request, Window *window, const ImVec2 &size, const ImVec2 &rescale) {

	if (_type == Type::Split &&
			(_childOne->_askForMerge || _childTwo->_askForMerge))
		merge();

	if (_type == Type::Split) {
		renderSplits(request, window, size, rescale);
	} else {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {6.f, 6.f});
		ImGui::BeginChild(_dockName.c_str(), size, ImGuiChildFlags_Borders,
						ImGuiWindowFlags_NoScrollbar);
		RenderDragDropContext	ctx(size);
		renderPanels(request, window, size);
		renderDragDrop(ctx);
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
}

void	Dock::renderSplits(RenderRequest *request, Window *window, const ImVec2 &size, const ImVec2 &rescale) {
	ImVec2	origin = ImGui::GetCursorScreenPos();
	bool	isVertical = (_splitDir == Splitter::Dir::Right);

	if (!_splitRatio)
		_splitRatio = 0.5f * (isVertical ? size.x : size.y);
	else if (_splitRatio < 0.f)
		_splitRatio = -*_splitRatio * (isVertical ? size.x : size.y);
	if (rescale.x > 0.f && rescale.y > 0.f)
		_splitRatio.value() *= isVertical ? rescale.x : rescale.y;

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

	_childOne->render(request, window, oneSize, rescale);
	ImGui::SetCursorScreenPos(isVertical ? ImVec2(start.x + oneSize.x, start.y)
										: ImVec2(start.x, start.y + oneSize.y));
	_childTwo->render(request, window, twoSize, rescale);
}

Dock::RenderDragDropContext::RenderDragDropContext(const ImVec2 &size) {
	origin = ImGui::GetCursorScreenPos();
	mouse = ImGui::GetMousePos();
	tabBarH = ImGui::GetFrameHeight();

	topLeft = {origin.x, origin.y + tabBarH};
	topRight = {origin.x + size.x, origin.y + tabBarH};
	bottomLeft = {origin.x, origin.y + size.y};
	bottomRight = {origin.x + size.x, origin.y + size.y};
	center = {origin.x + size.x * 0.5f,
		origin.y + tabBarH + (size.y - tabBarH) * 0.5f};

	released = ImGui::IsMouseReleased(ImGuiMouseButton_Left);

	bgColor = ImGui::ColorConvertFloat4ToU32(
		ImGui::GetStyleColorVec4(ImGuiCol_DragDropTargetBg)
	);
	mainColor = ImGui::ColorConvertFloat4ToU32(
		ImGui::GetStyleColorVec4(ImGuiCol_DragDropTarget)
	);
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
		draw->AddTriangleFilled(zone.a, zone.b, zone.c, ctx.bgColor);
		if (ctx.released)	{ split(zone.dir, panel); }
		return (true);
	}
	return (false);
}

void	Dock::renderTabBarZone(const RenderDragDropContext &ctx,
							ImDrawList *draw, IPanel *panel) {
	auto	&rectMin = ctx.origin;
	auto	&rectMax = ctx.topRight;

	size_t	closestIdx = 0;
	float	closestDist = FLT_MAX;

	if (ctx.mouse.x >= rectMin.x && ctx.mouse.x <= rectMax.x &&
		ctx.mouse.y >= rectMin.y && ctx.mouse.y <= rectMax.y) {
		for (size_t i = 0; i < _gaps.size(); i++) {
			if (_gaps[i] < rectMax.x && std::abs(ctx.mouse.x - _gaps[i]) < closestDist) {
				closestDist = std::abs(ctx.mouse.x - _gaps[i]);
				closestIdx = i;
			}
		}
		draw->AddRectFilled({_gaps[closestIdx] - 1.f, rectMin.y},
							{_gaps[closestIdx] + 1.f, rectMax.y},
							ctx.mainColor);
		if (ctx.released)
			panel->changeOwner(this, closestIdx);
	}
}

void	Dock::renderDragDrop(const RenderDragDropContext &ctx) {
	auto	*payload = ImGui::GetDragDropPayload();
	if (!payload || !payload->IsDataType("TAB_MOVE"))
		return ;

	IPanel	*panel = *static_cast<IPanel**>(payload->Data);
	auto	draw = ImGui::GetWindowDrawList();

	if (panel->getOwner() == this && _panels.size() == 1)
		return ;
	if (renderTriangleZones(ctx, draw, panel))
		return ;
	renderTabBarZone(ctx, draw, panel);
}

void	Dock::newPanelPopup(void) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {4.f, 4.f});
	if (ImGui::BeginPopup("OPEN_NEW_TAB")) {
		ImGui::SeparatorText("Panels");
		for (auto &[panelName, factory]: _ui->getPanelRegistry()) {
			if (ImGui::Selectable(panelName.c_str()))
				factory(_ui, this);
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void	Dock::renderPanels(RenderRequest *request, Window *window, const ImVec2 &) {
	auto	prev = ImGui::GetStyle().ItemInnerSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, {10.f, 0.f});
	float	innerSpacing = ImGui::GetStyle().ItemInnerSpacing.x;
	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_NoTabListScrollingButtons)) {
		ImVec2	effectiveSize = ImGui::GetContentRegionAvail();
		_gaps.clear();
		_gaps.push_back(ImGui::GetCursorScreenPos().x);

		int	i = 0;
		for (auto panel: _panels) {
			ImGui::PushID(i++);
			bool	open = ImGui::BeginTabItem(panel->getLabel());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, prev);
			_gaps.push_back(ImGui::GetItemRectMax().x + innerSpacing / 2);
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				ImGui::SetDragDropPayload("TAB_MOVE", &panel, sizeof(IPanel *));
				ImGui::TextUnformatted(panel->getLabel());
				ImGui::EndDragDropSource();
			}
			float	btnWidth = 12.f;
			if (Button("x")
					.setPos({ImGui::GetItemRectMax().x - btnWidth / 2,
							ImGui::GetItemRectMin().y})
					.setSize({btnWidth, btnWidth})
					.setEndPos(ImGui::GetCursorScreenPos())
					.showOnHover(ImGui::IsItemHovered())
					.build())
				panel->shouldClose(true);
			if (open) {
				if (auto *viewport = dynamic_cast<SceneViewport *>(panel))
					viewport->mainRequest = request;
				panel->render(window, effectiveSize);
				ImGui::EndTabItem();
			}
			ImGui::PopStyleVar();
			ImGui::PopID();
		}
		newPanelPopup();
		if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing))
			ImGui::OpenPopup("OPEN_NEW_TAB");
		ImGui::EndTabBar();
	}
	_panels.erase(std::remove_if(_panels.begin(), _panels.end(),
		[](IPanel *panel){ return (panel->shouldClose()); }), _panels.end());
	ImGui::PopStyleVar();
	if (_panels.empty() && !ImGui::GetDragDropPayload())
		_askForMerge = true;
}

}
