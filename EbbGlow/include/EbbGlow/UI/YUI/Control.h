#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/UI/YUI/Transform.h>

namespace ebbglow::ui::yui
{
	struct ControlCom
	{
		std::optional<Rect> interactiveArea;
		bool isActive = true;
		bool isVisible = true;
		core::entity parent;
	};

	bool IsVisible(core::ComponentPool<ControlCom>* pool, core::entity id);

	bool IsControlActive(core::ComponentPool<ControlCom>* pool, core::entity id);

	std::vector<input::InputPoint> GetActiveInputPoints(
		core::ComponentPool<ControlCom>* ctrlPool,
		core::ComponentPool<TransformCom>* transPool, core::entity id);

	bool IsActivePoint(core::ComponentPool<ControlCom>* ctrlPool,
		core::ComponentPool<TransformCom>* trans, core::entity id, const Vec2& pointPos);

	void ControlAttachTo(ControlCom& child, core::entity parentId);
	void ControlAttachTo(core::World2D& world, core::entity child, core::entity parent);
}