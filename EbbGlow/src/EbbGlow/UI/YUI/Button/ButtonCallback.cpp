#include <EbbGlow/UI/YUI/Button/ButtonCallback.h>

namespace ebbglow::ui::yui
{
	void ButtonCallbackSystem::update()
	{
		callbacks->active()->forEach([this](core::entity id, ButtonCallback& act)
			{
				auto* msgList = world->getMessageManager()->getMessageList(id);
				if (!msgList) return;
				for (auto& msg : *msgList)
				{
					if (msg->getType() == pressedId && act.onPress) act.onPress(*world, id);
					else if (msg->getType() == clickedId && act.onClick) act.onClick(*world, id);
					else if (msg->getType() == cancelledId && act.onCancel) act.onCancel(*world, id);
				}
			});
	}
}