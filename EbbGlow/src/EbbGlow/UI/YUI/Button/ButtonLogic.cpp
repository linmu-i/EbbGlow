#include <EbbGlow/UI/YUI/Button/ButtonLogic.h>

namespace ebbglow::ui::yui
{
	void ButtonLogicSystem::update()
	{
		buttonLogics->active()->forEach([this](core::entity id, ButtonLogic& act)
			{
				auto& ina = *buttonLogics->inactive()->get(id);


				if (!IsControlActive(controls->active(), id))
				{
					ina.isHovered = false;
					ina.isPressed = false;
					ina.isClicked = false;
					ina.isCancelled = false;
					ina.isDown = false;
					return;
				}

				auto points = GetActiveInputPoints(controls->active(), transforms->active(), id);
				
				bool allReleased = true;
				bool newPressed = false;

				if (!points.empty())
				{
					ina.isHovered = true;
				}
				else
				{
					ina.isHovered = false;
					ina.isPressed = false;
					ina.isClicked = false;
					if (act.isDown) ina.isCancelled = true;
					else ina.isCancelled = false;
					ina.isDown = false;
					return;
				}

				if (act.isPressed)
				{
					ina.isPressed = false;
				}
				if (act.isClicked)
				{
					ina.isClicked = false;
				}
				if (act.isCancelled)
				{
					ina.isCancelled = false;
				}

				for (auto& point : points)
				{

					if (point.type == input::PointType::Mouse)
					{
						if (!act.isDown && input::MousePressed(input::MouseButton::Left))
						{
							newPressed = true;

						}

						if (act.isDown && (!input::MouseReleased(input::MouseButton::Left) || input::MouseDown(input::MouseButton::Left)))
						{
							allReleased = false;
						}
					}
					else
					{
						if (!act.isDown && input::PointPressed(point.id))
						{
							newPressed = true;

						}

						if (act.isDown && (!input::PointReleased(point.id) || input::PointDown(point.id)))
						{
							allReleased = false;
						}
					}
				}

				if (newPressed)
				{
					ina.isPressed = true;
					ina.isDown = true;
				}
				if (allReleased && act.isDown)
				{
					ina.isClicked = true;
					ina.isDown = false;
				}
			});
	}
}