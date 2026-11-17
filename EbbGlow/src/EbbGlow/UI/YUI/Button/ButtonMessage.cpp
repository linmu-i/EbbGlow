#include <EbbGlow/UI/YUI/Button/ButtonMessage.h>

namespace ebbglow::ui::yui
{
	void ButtonMessageSystem::update()
	{
		coms->active()->forEach([this](core::entity id, ButtonMessage& act)
			{
				auto& button = *buttons->active()->get(id);
				auto& ina = *coms->inactive()->get(id);

				if (button.isPressed)
				{
					auto request = ClickRequest
					{
						.priority = act.priority,
						.entity = id,
						.callback = [this, &ina](core::World2D& world, core::entity entity)
						{
							ina.pressed = true;
							world.getMessageManager()->addUnicastMessage(std::make_unique<ButtonPressedMessage>(entity, buttonPressedType), entity);
						}
					};
					clickDispatcher->addRequest(request);
				}

				if (act.pressed && button.isClicked)
				{
					ina.pressed = false;
					world->getMessageManager()->addUnicastMessage(std::make_unique<ButtonClickedMessage>(id, buttonClickedType), id);
				}

				if (act.pressed && button.isCancelled)
				{
					ina.pressed = false;
					world->getMessageManager()->addUnicastMessage(std::make_unique<ButtonCancelledMessage>(id, buttonCancelledType), id);
				}
			});
	}
}