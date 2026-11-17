#pragma once

#include <EbbGlow/UI/YUI/Transform.h>
#include <EbbGlow/Utils/Functions.h>

namespace ebbglow::ui::yui
{
	struct ViewPortCom
	{
		std::optional<Rect> rect;
		core::entity parent;
	};

	std::optional<Rect> GetFinalViewPort(
		core::ComponentPool<ViewPortCom>* vPool,
		core::ComponentPool<TransformCom>* transPool, core::entity id);

	void ViewPortAttachTo(ViewPortCom& child, core::entity parentId);
	void ViewPortAttachTo(core::World2D& world, core::entity child, core::entity parent);
}