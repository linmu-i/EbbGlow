#include <EbbGlow/UI/YUI/ScrollContainer/ScrollContainer.h>
#include <EbbGlow/Utils/Input.h>

namespace ebbglow::ui::yui
{
	void ScrollContainerSystem::update()
	{
		scPool->active()->forEach([this](core::entity id, ScrollContainer& act)
			{
				auto& ina = *scPool->inactive()->get(id);
				auto& panelIna = *transPool->inactive()->get(act.panelId);

				if (!IsControlActive(controlPool->active(), act.panelId)) return;

				panelIna.transform.position.y = act.offsetY;
				panelIna.transform.position.x = act.offsetX;
				
				Vec2 mousePos = input::MousePosition();
				if (!IsActivePoint(controlPool->active(), transPool->active(), act.panelId, mousePos)) return;
				if (act.direction == ScrollContainer::ScrollDirection::Vertical)
				{
					ina.offsetY += input::MouseWheelDelta() * act.speed;
					ina.offsetY = std::clamp(ina.offsetY, act.minOffsetY, act.maxOffsetY);
				}
				else if (act.direction == ScrollContainer::ScrollDirection::Horizontal)
				{
					ina.offsetX += input::MouseWheelDelta() * act.speed;
					ina.offsetX = std::clamp(ina.offsetX, act.minOffsetX, act.maxOffsetX);
				}
			});
	}

	void ApplyScrollContainer(core::World2D& world)
	{
		world.addPool<ScrollContainer>();
		world.addSystem(ScrollContainerSystem(world));
	}
}