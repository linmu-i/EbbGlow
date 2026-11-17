#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/Core/Message.h>
#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/Click/ClickDispatch.h>

#include <EbbGlow/UI/YUI/Button/ButtonLogic.h>

namespace ebbglow::ui::yui
{
	class ButtonPressedMessage : public core::MessageBase
	{
	public:
		ButtonPressedMessage(core::entity senderId, core::MessageTypeId typeId) : MessageBase(senderId, typeId) {}
	};
	class ButtonClickedMessage : public core::MessageBase
	{
	public:
		ButtonClickedMessage(core::entity senderId, core::MessageTypeId typeId) : MessageBase(senderId, typeId) {}
	};
	class ButtonCancelledMessage : public core::MessageBase
	{
	public:
		ButtonCancelledMessage(core::entity senderId, core::MessageTypeId typeId) : MessageBase(senderId, typeId) {}
	};

	struct ButtonMessage
	{
		ClickPriorityT priority;
		bool pressed = false;
	};

	class ButtonMessageSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ButtonMessage>* coms;
		core::DoubleComs<ButtonLogic>* buttons;
		ClickDispatchSystem* clickDispatcher;
		core::World2D* world;

		core::MessageTypeId buttonPressedType;
		core::MessageTypeId buttonClickedType;
		core::MessageTypeId buttonCancelledType;

	public:
		ButtonMessageSystem(core::World2D& world) : coms(world.getDoubleBuffer<ButtonMessage>()),
			buttons(world.getDoubleBuffer<ButtonLogic>()), clickDispatcher(world.getSystem<ClickDispatchSystem>()),
			buttonPressedType(world.getMessageManager()->getMessageTypeManager().getId<ButtonPressedMessage>()),
			buttonClickedType(world.getMessageManager()->getMessageTypeManager().getId<ButtonClickedMessage>()),
			buttonCancelledType(world.getMessageManager()->getMessageTypeManager().getId<ButtonCancelledMessage>()),
			world(&world) {}

		void update() override;
	};
}