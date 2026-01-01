#include <EbbGlow/Utils/Resource.h>
#include <EbbGlow/Core/World.h>
#include <EbbGlow/Utils/Functions.h>
#pragma once

#include <EbbGlow/Graphics/Graphics.h>
#include <string>

namespace ebbglow::ui
{
	struct TextBoxExCom
	{
		rsc::SharedFile fontData;
		rsc::SharedFont font;
		rsc::FontType fontType;
		rsc::SharedShader sdfShader;
		std::string text;

		Vec2 position;
		float textSize;
		float spacing;
		Color textColor;

		core::Layers* layer;
		uint8_t layerDepth;

		TextBoxExCom(const rsc::SharedFile& fontData, const std::string& text, Vec2 position, float textSize, float spacing, Color textColor, core::Layers* layer, uint8_t layerDepth, rsc::FontType fontType = rsc::FontType::Default, rsc::SharedShader sdfShader = gfx::GetDefaultSDFShader()) : fontData(fontData),
			text(text), position(position), textSize(textSize), spacing(spacing), textColor(textColor), layer(layer), layerDepth(layerDepth), fontType(fontType),
			font(utils::DynamicLoadFont(fontData, text, fontType == rsc::FontType::Default ? textSize : 96, fontType)), sdfShader(sdfShader){
		}
	};

	class TextBoxExDraw : public core::DrawBase
	{
	private:
		const TextBoxExCom& textBox;

	public:
		TextBoxExDraw(const TextBoxExCom& textBox) : textBox(textBox) {}
		void draw() override;
	};

	class TextBoxExSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<TextBoxExCom>* textBoxs;
		core::Layers* layer;

	public:
		TextBoxExSystem(core::World2D& world) : textBoxs(world.getDoubleBuffer<TextBoxExCom>()), layer(world.getUiLayer()) {}
		void update() override;
	};

	inline void ApplyTextBoxEx(core::World2D& world)
	{
		world.addPool<TextBoxExCom>();
		world.addSystem(TextBoxExSystem(world));
	}
}