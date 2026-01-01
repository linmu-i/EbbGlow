#include <EbbGlow/UI/Button/ButtonEx.h>
#include <EbbGlow/UI/Button/ButtonMsg.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Functions.h>
#include <EbbGlow/Utils/Math.h>
#include <EbbGlow/Utils/Input.h>

namespace ebbglow::ui
{
	void ButtonExDraw::draw()
	{
		const rsc::SharedTexture2D* iconRef;
		switch (icon)
		{
		case 0:
			iconRef = &button.pressedIcon;
			break;
		case 1:
			iconRef = &button.hoverIcon;
			break;
		default:
			iconRef = &button.normalIcon;
		}
		if (iconRef->valid())
		{
			gfx::DrawTexture(*iconRef, button.rect.position(), button.iconScale);
		}
		if (button.font.valid())
		{
			gfx::DrawText(button.font, button.text, button.rect.position() + button.rect.coverage() / 2 - utils::MeasureTextSize(button.font, button.text, button.textSize, button.spacing) / 2, button.textSize, button.spacing);
		}
	}

	void ButtonExSystem::update()
	{
		Vec2 mousePos = input::MousePosition();
		buttons->active()->forEach([&mousePos, this](core::entity id, ButtonExCom& act)
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
				if (!act.pressed)
				{
					if (press && inBoxCount == 1)
					{
						ina.pressed = true;
						msgMgr->addUnicastMessage(std::make_unique<ButtonPressMsg>(id, pressId), id);
					}
				}
				else
				{
					if (inBoxCount == 0 || release)
					{
						ina.pressed = false;
						if (release) msgMgr->addUnicastMessage(std::make_unique<ButtonReleaseMsg>(id, releaseId), id);
					}
				}
				uint8_t icon;
				if (inBoxCount > 0)
				{
					if (act.pressed)
					{
						icon = 0;
					}
					else
					{
						icon = 1;
					}
				}
				else
				{
					icon = 2;
				}
				(*act.layer)[act.layerDepth].push_back(std::make_unique<ButtonExDraw>(act, icon));
			});
	}
}