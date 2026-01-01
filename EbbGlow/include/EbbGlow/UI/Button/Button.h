#pragma once

#include <string>

#include <EbbGlow/Utils/Resource.h>
#include <EbbGlow/Core/ECS.h>
#include <EbbGlow/Core/World.h>
#include <EbbGlow/Core/Message.h>
#include <EbbGlow/UI/Button/ButtonMsg.h>

namespace ebbglow::ui
{
	struct ButtonCom
	{
		rsc::SharedTexture2D icon;
		rsc::SharedFont font;
		Color textColor;
		Color color;
		std::string text;
		float textSize;
		Rect rect;

		bool press;
		float iconScale;

		uint8_t layerDepth;

		ButtonCom(Rect rect, rsc::SharedTexture2D icon, float iconScale, Color color, rsc::SharedFont font, Color textColor, const std::string& text, float textSize, uint8_t layerDepth) :
			rect(rect), icon(icon), iconScale(iconScale), color(color), font(font), textColor(textColor), text(text), textSize(textSize), layerDepth(layerDepth), press(false) {
		}

		ButtonCom(Rect rect, Color color, rsc::SharedFont font, Color textColor, const std::string& text, float textSize, uint8_t layerDepth) :
			rect(rect), icon(rsc::SharedTexture2D()), iconScale(0.0f), color(color), font(font), textColor(textColor), text(text), textSize(textSize), layerDepth(layerDepth), press(false) {
		}

		ButtonCom(Rect rect, rsc::SharedFont font, Color textColor, const std::string& text, float textSize, uint8_t layerDepth) :
			rect(rect), icon(rsc::SharedTexture2D()), iconScale(0.0f), color(colors::Blank), font(font), textColor(textColor), text(text), textSize(textSize), layerDepth(layerDepth), press(false) {
		}
	};

	class ButtonDraw : public core::DrawBase
	{
	private:
		ButtonCom& button;
	public:
		ButtonDraw(ButtonCom& buttonRef) : button(buttonRef) {}
		void draw() override;
	};

	class ButtonSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ButtonCom>* buttons;
		core::MessageManager* msgMgr;
		core::Layers* layer;

		core::MessageTypeId pressId;
		core::MessageTypeId releaseId;
	public:
		ButtonSystem(core::World2D& world) : buttons(world.getDoubleBuffer<ButtonCom>()), msgMgr(world.getMessageManager()), layer(world.getUiLayer()),
			pressId(world.getMessageManager()->getMessageTypeManager().getId<ButtonPressMsg>()), releaseId(world.getMessageManager()->getMessageTypeManager().getId<ButtonReleaseMsg>()) {}
		void update() override;
	};

	inline void ApplyButton(core::World2D& world)
	{
		world.getMessageManager()->getMessageTypeManager().registeredType<ButtonPressMsg>();
		world.getMessageManager()->getMessageTypeManager().registeredType<ButtonHoverMsg>();
		world.getMessageManager()->getMessageTypeManager().registeredType<ButtonReleaseMsg>();
		world.addPool<ButtonCom>();
		world.addSystem(ButtonSystem(world));
	}
}