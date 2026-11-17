#include <EbbGlow/UI/YUI/View.h>

namespace ebbglow::ui::yui
{
	std::optional<Rect> GetFinalViewPort(
		core::ComponentPool<ViewPortCom>* vPool,
		core::ComponentPool<TransformCom>* transPool, core::entity id)
	{
		if (!vPool || !transPool) return std::nullopt;

		bool hasValue = false;
		auto actId = id;
		Rect result{Vec2::Zero(), utils::ScreenSize()};
		while (actId != core::InvalidEntity)
		{
			auto* viewPort = vPool->get(actId);
			if (!viewPort) break;
			if (viewPort->rect)
			{
				Transform finalTrans = GetFinalTransform(GetTransforms(transPool, actId));
				result = result.intersection(
					viewPort->rect
					->scaleAround(finalTrans.pivot, finalTrans.scale)
					.offsetOf(finalTrans.position));
				hasValue = true;
			}
			actId = viewPort->parent;
		}
		
		return hasValue ? std::make_optional(result) : std::nullopt;
	}

	void ViewPortAttachTo(ViewPortCom& child, core::entity parentId)
	{
		child.parent = parentId;
	}

	void ViewPortAttachTo(core::World2D& world, core::entity child, core::entity parent)
	{
		auto* c = world.getDoubleBuffer<ViewPortCom>()->inactive()->get(child);
		if (!c)
		{
			c = world.getWaitAdd<ViewPortCom>(child);
		}
		if (!c) return;
		ViewPortAttachTo(*c, parent);
	}
}