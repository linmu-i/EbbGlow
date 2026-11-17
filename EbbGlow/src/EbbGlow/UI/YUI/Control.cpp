#include <EbbGlow/UI/YUI/Control.h>

namespace ebbglow::ui::yui
{
	bool IsVisible(core::ComponentPool<ControlCom>* pool, core::entity id)
	{
		if (!pool) return true;
		core::entity actId = id;
		while (actId != core::InvalidEntity)
		{
			auto* controlCom = pool->get(actId);
			if (!controlCom) return true;
			if (!controlCom->isVisible) return false;
			actId = controlCom->parent;
		}
		return true;
	}

	bool IsControlActive(core::ComponentPool<ControlCom>* pool, core::entity id)
	{
		if (!pool) return true;
		core::entity actId = id;
		while (actId != core::InvalidEntity)
		{
			auto* controlCom = pool->get(actId);
			if (!controlCom) return true;
			if (!controlCom->isActive) return false;
			actId = controlCom->parent;
		}
		return true;
	}

	std::vector<input::InputPoint> GetActiveInputPoints(
		core::ComponentPool<ControlCom>* ctrlPool,
		core::ComponentPool<TransformCom>* transPool, core::entity id)
	{
		if (!ctrlPool || !transPool) return input::PointList();

		auto IsActPoint = [](Transform finalTransform, Rect activeArea, Vec2 point)
			{

				Vec2 finalPoint = (point - finalTransform.position).rotatedAround(finalTransform.pivot, -finalTransform.rotation);
				return activeArea.scaleAround(finalTransform.pivot, finalTransform.scale).contain(finalPoint);
			};

		std::vector<input::InputPoint> result = input::PointList();
		core::entity actId = id;
		while (actId != core::InvalidEntity)
		{
			auto* control = ctrlPool->get(actId);
			if (!control) break;
			
			if (control->interactiveArea.has_value())
			{
				Transform finalTransform = GetFinalTransform(GetTransforms(transPool, actId));

				result.erase(std::remove_if(result.begin(), result.end(),
					[&](const input::InputPoint& point)
					{
						return !IsActPoint(finalTransform, *control->interactiveArea, point.position);
					}), result.end());
				if (result.empty()) return {};
			}

			actId = control->parent;
		}
		return result;
	}

	bool IsActivePoint(core::ComponentPool<ControlCom>* ctrlPool,
		core::ComponentPool<TransformCom>* trans, core::entity id, const Vec2& pointPos)
	{
		if (!ctrlPool || !trans) return true;

		auto IsActPoint = [](Transform finalTransform, Rect activeArea, Vec2 point)
			{

				Vec2 finalPoint = (point - finalTransform.position).rotatedAround(finalTransform.pivot, -finalTransform.rotation);
				return activeArea.scaleAround(finalTransform.pivot, finalTransform.scale).contain(finalPoint);
			};

		core::entity actId = id;
		while (actId != core::InvalidEntity)
		{
			if (auto* control = ctrlPool->get(actId))
			{
				if (control->interactiveArea.has_value())
				{
					Transform finalTransform = GetFinalTransform(GetTransforms(trans, actId));
					if (!IsActPoint(finalTransform, *control->interactiveArea, pointPos))
					{
						return false;
					}
				}
				actId = control->parent;
			}
			else
			{
				break;
			}
		}
		return true;
	}

	void ControlAttachTo(ControlCom& child, core::entity parentId)
	{
		child.parent = parentId;
	}

	void ControlAttachTo(core::World2D& world, core::entity child, core::entity parent)
	{
		auto* c = world.getDoubleBuffer<ControlCom>()->inactive()->get(child);
		if (!c)
		{
			c = world.getWaitAdd<ControlCom>(child);
		}
		if (!c) return;
		ControlAttachTo(*c, parent);
	}
}