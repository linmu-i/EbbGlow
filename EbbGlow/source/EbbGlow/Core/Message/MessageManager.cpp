#include <EbbGlow/Core/Message/MessageManager.h>
#include <EbbGlow/Utils/ThreadPool.h>

namespace
{
	void MessageUnicast(ebbglow::core::MessageBase* msg, ebbglow::core::entity target, ebbglow::core::ComponentPool<std::vector<ebbglow::core::MessageBase*>>& msgList)
	{
		auto list = msgList.get(target);
		if (list != nullptr)
		{
			list->push_back(msg);
		}
		else
		{
			msgList.add(target, std::vector<ebbglow::core::MessageBase*>());
			msgList.get(target)->push_back(msg);
		}
	}

	void MessageMulticast(ebbglow::core::MessageBase* msg, const std::vector<ebbglow::core::entity>& target, ebbglow::core::ComponentPool<std::vector<ebbglow::core::MessageBase*>>& msgList)
	{
		for (auto t : target)
		{
			auto list = msgList.get(t);
			if (list != nullptr)
			{
				list->push_back(msg);
			}
			else
			{
				msgList.add(t, std::vector<ebbglow::core::MessageBase*>());
				msgList.get(t)->push_back(msg);
			}
		}
	}

	void MessageBroadcast(ebbglow::core::MessageBase* msg, ebbglow::core::ComponentPool<std::vector<ebbglow::core::MessageBase*>>& msgList)
	{
		msgList.forEach
		(
			[msg](ebbglow::core::entity id, std::vector<ebbglow::core::MessageBase*>& list)
			{
				list.push_back(msg);
			}
		);
	}
}

namespace ebbglow::core
{
	MessageManager& MessageManager::addUnicastMessage(std::unique_ptr<MessageBase>&& msg, entity targetId)
	{
		std::lock_guard lock(mutex);
		auto& l = unicastInactive();
		l.push_back(std::move(msg));
		unicastTargetListInactive().push_back(targetId);
		return *this;
	}

	MessageManager& MessageManager::addMulticastMessage(std::unique_ptr<MessageBase>&& msg, const std::vector<entity>& targetIds)
	{
		std::lock_guard lock(mutex);
		auto& l = multicastInactive();
		l.push_back(std::move(msg));
		multicastTargetListInactive().push_back(targetIds);
		return *this;
	}

	MessageManager& MessageManager::addBroadcastMessage(std::unique_ptr<MessageBase>&& msg)
	{
		std::lock_guard lock(mutex);
		auto& l = broadcastInactive();
		l.push_back(std::move(msg));
		return *this;
	}

	std::vector<MessageBase*>* MessageManager::getMessageList(entity id)
	{
		std::lock_guard lock(mutex);
		return messageList.get(id);
	}

	void MessageManager::subscribe(entity id)//订阅消息，任何未订阅消息的实体不会收到任何消息，订阅后将接收广播消息，作为目标时接收组播/单播消息
	{
		messageList.add(id, std::vector<MessageBase*>());
	}

	void MessageManager::unsubscribe(entity id)//取消订阅，将不会收到任何消息
	{
		messageList.remove(id);
	}

	void MessageManager::sendAll()//发送所有消息到目标消息列表
	{
		std::lock_guard lock(mutex);
		for (size_t i = 0; i < unicastActive().size(); ++i)
		{
			MessageUnicast(unicastActive()[i].get(), unicastTargetListActive()[i], messageList);
		}
		for (size_t i = 0; i < multicastActive().size(); ++i)
		{
			MessageMulticast(multicastActive()[i].get(), multicastTargetListActive()[i], messageList);
		}
		for (size_t i = 0; i < broadcastActive().size(); ++i)
		{
			MessageBroadcast(broadcastActive()[i].get(), messageList);
		}
	}

	void MessageManager::sendAll(utils::ThreadPool& tp)//发送所有消息到目标消息列表
	{
		std::lock_guard lock(mutex);

		auto r1 = tp.enqueue
		(
			[this]()
			{
				for (size_t i = 0; i < unicastActive().size(); ++i)
				{
					MessageUnicast(unicastActive()[i].get(), unicastTargetListActive()[i], messageList);
				}
			}
		);
		auto r2 = tp.enqueue
		(
			[this]()
			{
				for (size_t i = 0; i < multicastActive().size(); ++i)
				{
					MessageMulticast(multicastActive()[i].get(), multicastTargetListActive()[i], messageList);
				}
			}
		);
		auto r3 = tp.enqueue
		(
			[this]()
			{
				for (size_t i = 0; i < broadcastActive().size(); ++i)
				{
					MessageBroadcast(broadcastActive()[i].get(), messageList);
				}
			}
		);
		r1.wait();
		r2.wait();
		r3.wait();
	}

	void MessageManager::swap()//清除活跃消息及已发送消息队列，并切换活跃区
	{
		std::lock_guard lock(mutex);
		messageList.forEach
		(
			[](entity id, std::vector<MessageBase*>& list) {list.clear(); }
		);
		unicastActive().clear();
		multicastActive().clear();
		broadcastActive().clear();

		unicastTargetListActive().clear();
		multicastTargetListActive().clear();


		active = !active;
	}
}