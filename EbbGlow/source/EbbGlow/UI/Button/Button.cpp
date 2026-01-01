#include <EbbGlow/UI/Button/Button.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/UI/Button/ButtonMsg.h>
#include <EbbGlow/Utils/Functions.h>
#include <EbbGlow/Utils/Math.h>

namespace ebbglow::ui
{
	void ButtonDraw::draw()
	{
		if (button.icon.valid())
		{
			gfx::DrawTexture(button.icon, button.rect.position(), 0.0f, button.iconScale);
		}
		else
		{
			gfx::DrawRect(button.rect, button.color);
		}
		if (button.font.valid())
		{
			gfx::DrawText(button.font, button.text, button.rect.position() + button.rect.coverage() / 2 - utils::MeasureTextSize(button.font, button.text, button.textSize, button.textSize / 10) / 2, button.textSize, button.textSize / 10);
		}
	}

	void ButtonSystem::update()
	{
		Vec2 mousePos = input::MousePosition();
		buttons->active()->forEach([&mousePos, this](core::entity id, ButtonCom& act)
			{
				auto& ina = *buttons->inactive()->get(id);
				bool press = false;
				bool release = false;
				int inBoxCount = 0;
				for (auto& p : input::PointList())
				{
					if (act.rect.inBox(p.position))
					{
						if (p.type == input::PointType::Mouse)
						{
							if (input::MousePressed(input::MouseButton::Left))
							{
								press = true;
							}
							if (input::MouseReleased(input::MouseButton::Left))
							{
								release = true;
							}
						}
						else
						{
							if (input::PointPressed(p.id))
							{
								press = true;
							}
							if (input::PointReleased(p.id))
							{
								release = true;
							}
						}
						++inBoxCount;
					}
				}
				if (!act.press)
				{
					if (press && inBoxCount == 1)
					{
						ina.press = true;
						msgMgr->addUnicastMessage(std::make_unique<ButtonPressMsg>(ButtonPressMsg(id, pressId)), id);
					}
				}
				else
				{
					if (inBoxCount == 0)
					{
						ina.press = false;
						if (release) msgMgr->addUnicastMessage(std::make_unique<ButtonReleaseMsg>(ButtonReleaseMsg(id, releaseId)), id);
					}
				}
				(*layer)[act.layerDepth].push_back(std::make_unique<ButtonDraw>(ButtonDraw(act)));
			});
	}
}