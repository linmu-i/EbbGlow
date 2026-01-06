#pragma once

#include <EbbGlow/UI/Button/ButtonMsg.h>
#include <EbbGlow/Utils/Types.h>
#include <EbbGlow/Utils/Resource.h>
#include <EbbGlow/Core/World.h>

namespace ebbglow::ui
{
	struct ButtonExCom
	{
		Rect rect;
		
		rsc::SharedTexture2D normalIcon;
		rsc::SharedTexture2D hoverIcon;
		rsc::SharedTexture2D pressedIcon;
		float iconScale;

		bool pressed;

		rsc::SharedFont font;
		std::string text;
		Color textColor;
		float textSize;
		float spacing;

		core::Layer* layer;

		ButtonExCom(Rect rect, uint8_t layerDepth, core::Layers* layer, const rsc::SharedTexture2D& normalIcon, const rsc::SharedTexture2D& hoverIcon, const rsc::SharedTexture2D& pressedIcon, float iconScale, const rsc::SharedFont& font, const std::string& text, Color textColor, float textSize, float spacing) :
			rect(rect), normalIcon(normalIcon), hoverIcon(hoverIcon), pressedIcon(pressedIcon), iconScale(iconScale), font(font), text(text), textColor(textColor), textSize(textSize), spacing(spacing), layer(&(*layer)[layerDepth]), pressed(false) {
		}

		ButtonExCom(Rect rect, core::Layer* layer, const rsc::SharedTexture2D& normalIcon, const rsc::SharedTexture2D& hoverIcon, const rsc::SharedTexture2D& pressedIcon, float iconScale, const rsc::SharedFont& font, const std::string& text, Color textColor, float textSize, float spacing) :
			rect(rect), normalIcon(normalIcon), hoverIcon(hoverIcon), pressedIcon(pressedIcon), iconScale(iconScale), font(font), text(text), textColor(textColor), textSize(textSize), spacing(spacing), layer(layer), pressed(false) {
		}
	};

	class ButtonExDraw : public core::DrawBase
	{
	private:
		uint8_t icon;
		const ButtonExCom& button;

	public:
		ButtonExDraw(const ButtonExCom& button, uint8_t icon) : icon(icon), button(button) {}

		void draw() override;
	};

	class ButtonExSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ButtonExCom>* buttons;
		core::MessageManager* msgMgr;

		core::MessageTypeId pressId;
		core::MessageTypeId releaseId;

	public:
		ButtonExSystem(core::World2D& world) : buttons(world.getDoubleBuffer<ButtonExCom>()), msgMgr(world.getMessageManager()),
			pressId(world.getMessageManager()->getMessageTypeManager().getId<ButtonPressMsg>()), releaseId(world.getMessageManager()->getMessageTypeManager().getId<ButtonReleaseMsg>()) {}

		ButtonExSystem(core::DoubleComs<ButtonExCom>* buttons, core::World2D& world) : buttons(buttons), msgMgr(world.getMessageManager()),
			pressId(world.getMessageManager()->getMessageTypeManager().getId<ButtonPressMsg>()), releaseId(world.getMessageManager()->getMessageTypeManager().getId<ButtonReleaseMsg>()) {
		}

		void update() override;
	};

	inline void ApplyButtonEx(core::World2D& world)
	{
		world.getMessageManager()->getMessageTypeManager().registeredType<ButtonPressMsg>();
		world.getMessageManager()->getMessageTypeManager().registeredType<ButtonHoverMsg>();
		world.getMessageManager()->getMessageTypeManager().registeredType<ButtonReleaseMsg>();
		world.addPool<ButtonExCom>();
		world.addSystem(ButtonExSystem(world));
	}
}