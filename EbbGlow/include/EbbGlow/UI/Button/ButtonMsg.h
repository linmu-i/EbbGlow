#pragma once

#include <EbbGlow/Core/Message.h>

namespace ebbglow::ui
{
	class ButtonPressMsg : public core::MessageBase
	{
	public:
		ButtonPressMsg(core::entity senderId, core::MessageTypeId typeId) : MessageBase(senderId, typeId) {}
	};

	class ButtonReleaseMsg : public core::MessageBase
	{
	public:
		ButtonReleaseMsg(core::entity senderId, core::MessageTypeId typeId) : MessageBase(senderId, typeId) {}
	};

	class ButtonHoverMsg : public core::MessageBase
	{
	public:
		ButtonHoverMsg(core::entity senderId, core::MessageTypeId typeId) : MessageBase(senderId, typeId) {}
	};
}