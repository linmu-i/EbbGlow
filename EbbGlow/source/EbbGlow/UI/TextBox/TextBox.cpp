#include <EbbGlow/UI/TextBox/TextBox.h>
#include <EbbGlow/Graphics/Graphics.h>

namespace ebbglow::ui
{
	void TextBoxDraw::draw()
	{
		gfx::DrawText(textBox.font, textBox.text, textBox.position, textBox.textSize, textBox.spacing, textBox.textColor);
	}

	void TextBoxSystem::update()
	{
		textBoxs->active()->forEach([this](core::entity id, TextBoxCom& act)
			{
				auto& ina = *textBoxs->inactive()->get(id);
				if (act.font.valid()) (*layer)[act.layerDepth].push_back(std::make_unique<TextBoxDraw>(act));
			});
	}
}