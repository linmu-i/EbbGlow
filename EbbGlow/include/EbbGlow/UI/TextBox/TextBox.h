#pragma once

#include <EbbGlow/Utils/Resource.h>
#include <EbbGlow/Core/World.h>
#include <string>

namespace ebbglow::ui
{
	struct TextBoxCom
	{
		rsc::SharedFont font;
		std::string text;

		Vec2 position;
		float textSize;
		float spacing;
		Color textColor;

		uint8_t layerDepth;
	};

	class TextBoxDraw : public core::DrawBase
	{
	private:
		const TextBoxCom& textBox;

	public:
		TextBoxDraw(const TextBoxCom& textBox) : textBox(textBox) {}

		void draw() override;
	};

	class TextBoxSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<TextBoxCom>* textBoxs;
		core::Layers* layer;

	public:
		TextBoxSystem(core::World2D& world) : textBoxs(world.getDoubleBuffer<TextBoxCom>()), layer(world.getUiLayer()) {}

		void update() override;
	};

	inline void ApplyTextBox(core::World2D& world)
	{
		world.addPool<TextBoxCom>();
		world.addSystem(TextBoxSystem(world));
	}
}