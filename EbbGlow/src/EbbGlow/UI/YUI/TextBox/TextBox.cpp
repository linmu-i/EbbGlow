#include <EbbGlow/UI/YUI/TextBox/TextBox.h>

#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::ui::yui
{
	void TextBoxDraw::draw()
	{
		if (!textBox.font)
			return;

		if (viewPort.has_value()) BeginScissorMode(*viewPort);
		Vec2 finalPos = (transform.position + transform.pivot * (1 - transform.scale));
		
		gfx::DrawText(textBox.font, textBox.text, finalPos, textBox.textSize * transform.scale, textBox.spacing * transform.scale, textBox.textColor, transform.pivot * transform.scale, transform.rotation);
		if (viewPort.has_value()) EndScissorMode();
	}

	void TextBoxSystem::update()
	{
		textBox->active()->forEach([this](core::entity id, TextBox& act)
			{
				auto& ina = *textBox->inactive()->get(id);
				ina = act;

				if (!IsVisible(control->active(), id)) return;
				Transform finalTrans = GetFinalTransform(GetTransforms(trans->active(), id));
				auto viewPortOpt = GetFinalViewPort(viewPort->active(), trans->active(), id);

				auto* layerCom = layer->active()->get(id);
				layerCom->layer->add(std::make_unique<TextBoxDraw>(act, finalTrans, viewPortOpt), layerCom->priority);
			});
	}

	void ApplyTextBox(core::World2D& world)
	{
		world.addPool<TextBox>();
		world.addSystem(TextBoxSystem(world));
	}
}