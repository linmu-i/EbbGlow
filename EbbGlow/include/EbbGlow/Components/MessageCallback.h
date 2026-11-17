#pragma once

#include <EbbGlow/Core/Message.h>
#include <EbbGlow/UI/YUI/Button/ButtonMessage.h>
#include <EbbGlow/Core/World.h>

namespace ebbglow::components
{
	struct MessageCallback
	{
		core::MessageManager* msgMgr;
		std::vector<std::function<void(core::MessageBase*)>> callbacks;
		MessageCallback(core::MessageManager* msgMgr, const std::vector<std::function<void(core::MessageBase*)>>& callbacks) :
			msgMgr(msgMgr), callbacks(callbacks) {}
	};

	class MessageCallbackSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<MessageCallback>* coms;

	public:
		MessageCallbackSystem(core::DoubleComs<MessageCallback>* coms) : coms(coms){}
		void update() override;
	};

	inline void ApplyMessageCallback(core::World2D& world)
	{
		world.addPool<MessageCallback>();
		world.addSystem(MessageCallbackSystem(world.getDoubleBuffer<MessageCallback>()));
	}
}