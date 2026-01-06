#include <EbbGlow/UI/TextBox/TextBoxEx.h>

namespace ebbglow::ui
{
	void TextBoxExDraw::draw()
	{
		if (textBox.fontType == rsc::FontType::Sdf)
		{
			gfx::DrawSDFText(textBox.font, textBox.text, textBox.position, textBox.textSize, textBox.spacing, textBox.textColor, { 0.0f, 0.0f }, 0.0f, textBox.sdfShader.valid() ? textBox.sdfShader : gfx::GetDefaultSDFShader());
		}
		else
		{
			gfx::DrawText(textBox.font, textBox.text, textBox.position, textBox.textSize, textBox.spacing, textBox.textColor);
		}
	}

	void TextBoxExSystem::update()
	{
		textBoxes->active()->forEach([this](core::entity id, TextBoxExCom& act)
			{
				(*act.layer).push_back(std::make_unique<TextBoxExDraw>(act));
			});
	}
}