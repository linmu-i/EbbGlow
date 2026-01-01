#pragma once

#include <EbbGlow/Core/ECS.h>

namespace ebbglow::core
{
	using MessageTypeId = uint64_t;

	class MessageBase
	{
	private:
		entity senderId;
		MessageTypeId type;
	public:
		MessageBase(entity senderId, MessageTypeId typeId) : senderId(senderId), type(typeId) {}
		entity getSender() const { return senderId; }
		MessageTypeId getType() const { return type; }
	};

	using MessageList = ComponentPool<std::vector<MessageBase*>>;
}