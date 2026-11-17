#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/Utils/Resource.h>

#include <EbbGlow/UI/YUI/YUIBasic.h>

namespace ebbglow::ui::yui
{
	struct TextBox
	{
		std::string text;
		float textSize = 32.0f;
		float spacing = 3.0f;
		rsc::SharedFont font;
		Color textColor;
	};

	class TextBoxDraw : public core::DrawBase
	{
	private:
		TextBox& textBox;
		Transform transform;
		std::optional<Rect> viewPort;

	public:
		TextBoxDraw(TextBox& textBox, Transform transform, std::optional<Rect> viewPort) : textBox(textBox),
			transform(transform), viewPort(viewPort) {
		}

		void draw() override;
	};

	class TextBoxSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<TransformCom>* trans;
		core::DoubleComs<ControlCom>* control;
		core::DoubleComs<ViewPortCom>* viewPort;
		core::DoubleComs<LayerCom>* layer;
		core::DoubleComs<TextBox>* textBox;
	public:
		TextBoxSystem(core::DoubleComs<TransformCom>* trans, core::DoubleComs<ControlCom>* control, core::DoubleComs<ViewPortCom>* viewPort, core::DoubleComs<LayerCom>* layer, core::DoubleComs<TextBox>* textBox)
			: trans(trans), control(control), viewPort(viewPort), layer(layer), textBox(textBox) {}

		TextBoxSystem(core::World2D& world) :
			trans(world.getDoubleBuffer<TransformCom>()), control(world.getDoubleBuffer<ControlCom>()),
			viewPort(world.getDoubleBuffer<ViewPortCom>()), layer(world.getDoubleBuffer<LayerCom>()),
			textBox(world.getDoubleBuffer<TextBox>()) {}

		void update() override;
	};

	void ApplyTextBox(core::World2D& world);
}