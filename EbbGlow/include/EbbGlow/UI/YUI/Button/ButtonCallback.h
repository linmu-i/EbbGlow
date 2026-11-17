#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/Button/ButtonMessage.h>

namespace ebbglow::ui::yui
{
	struct ButtonCallback
	{
		std::function<void(core::World2D&, core::entity)> onPress;
		std::function<void(core::World2D&, core::entity)> onClick;
		std::function<void(core::World2D&, core::entity)> onCancel;
	};

	class ButtonCallbackSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ButtonCallback>* callbacks;
		core::World2D* world;

		core::MessageTypeId pressedId;
		core::MessageTypeId clickedId;
		core::MessageTypeId cancelledId;

	public:
		ButtonCallbackSystem(core::World2D& world) :
			callbacks(world.getDoubleBuffer<ButtonCallback>()), world(&world),
			pressedId(world.getMessageManager()->getMessageTypeManager().getId<ButtonPressedMessage>()),
			clickedId(world.getMessageManager()->getMessageTypeManager().getId<ButtonClickedMessage>()),
			cancelledId(world.getMessageManager()->getMessageTypeManager().getId<ButtonCancelledMessage>()) {}
		
		void update() override;
	};
}