#pragma once

#include "ECS.h"

namespace ecs
{
	using MessageTypeId = uint64_t;

	enum class MessageDeliverType : uint8_t
	{
		Unicast,
		Multicast,
		Broadcast
	};

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

	void MessageUnicast(MessageBase* msg, entity target, ComponentPool<std::vector<MessageBase*>>& msgList)
	{
		auto list = msgList.get(target);
		if (list != nullptr)
		{
			list->push_back(msg);
		}
		else
		{
			msgList.add(target, std::vector<MessageBase*>());
			msgList.get(target)->push_back(msg);
		}
	}
	void MessageMulticast(MessageBase* msg, const std::vector<entity>& target, ComponentPool<std::vector<MessageBase*>>& msgList)
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
				msgList.add(t, std::vector<MessageBase*>());
				msgList.get(t)->push_back(msg);
			}
		}
	}
	void MessageBroadcast(MessageBase* msg, ComponentPool<std::vector<MessageBase*>>& msgList)
	{
		msgList.forEach
		(
			[msg](entity id, std::vector<MessageBase*>& list)
			{
				list.push_back(msg);
			}
		);
	}

	class MessageTypeManager
	{
	private:
		IdManager idManager;
		std::unordered_map<std::type_index, MessageTypeId> typeToId;
		std::vector<std::type_index> idToType;

	public:
		template<typename T>
		//using T = int;
		MessageTypeId registeredType()
		{
			auto it = typeToId.find(std::type_index(typeid(T)));
			if (it != typeToId.end())
			{
				return it->second;//已有项不重复注册
			}
			else
			{
				MessageTypeId id = idManager.getId();
				typeToId.emplace(std::type_index(typeid(T)), id);

				if (id >= idToType.size())
				{
					idToType.resize(id + 10, std::type_index(typeid(void)));//预分配
				}
				idToType[id] = std::type_index(typeid(T));

				return id;
			}
		}
		std::type_index getType(MessageTypeId id)
		{
			if (id < idToType.size())
			{
				return idToType[id];
			}
			else
			{
				return std::type_index(typeid(void));
			}
		}
		template<typename T>
		MessageTypeId getId()
		{
			auto it = typeToId.find(std::type_index(typeid(T)));
			if (it != typeToId.end())
			{
				return it->second;
			}
			else
			{
				return SIZE_MAX;
			}
		}
	};

	class MessageManager
	{
	private:
		std::vector<std::unique_ptr<MessageBase>> msgUnicastList0;
		std::vector<std::unique_ptr<MessageBase>> msgUnicastList1;
		std::vector<entity> unicastTargetList0;
		std::vector<entity> unicastTargetList1;

		std::vector<std::unique_ptr<MessageBase>> msgMulticastList0;
		std::vector<std::unique_ptr<MessageBase>> msgMulticastList1;
		std::vector<std::vector<entity>> multicastTargetList0;
		std::vector<std::vector<entity>> multicastTargetList1;

		std::vector<std::unique_ptr<MessageBase>> msgBroadcastList0;
		std::vector<std::unique_ptr<MessageBase>> msgBroadcastList1;

		bool active = true;

		std::mutex mutex;

		MessageTypeManager messageTypeManager;

		std::vector<std::unique_ptr<MessageBase>>& unicastActive() { return active ? msgUnicastList0 : msgUnicastList1; }
		std::vector<std::unique_ptr<MessageBase>>& unicastInactive() { return active ? msgUnicastList1 : msgUnicastList0; }

		std::vector<std::unique_ptr<MessageBase>>& multicastActive() { return active ? msgMulticastList0 : msgMulticastList1; }
		std::vector<std::unique_ptr<MessageBase>>& multicastInactive() { return active ? msgMulticastList1 : msgMulticastList0; }

		std::vector<std::unique_ptr<MessageBase>>& broadcastActive() { return active ? msgBroadcastList0 : msgBroadcastList1; }
		std::vector<std::unique_ptr<MessageBase>>& broadcastInactive() { return active ? msgBroadcastList1 : msgBroadcastList0; }

		std::vector<entity>& unicastTargetListActive() { return active ? unicastTargetList0 : unicastTargetList1; }
		std::vector<entity>& unicastTargetListInactive() { return active ? unicastTargetList1 : unicastTargetList0; }

		std::vector<std::vector<entity>>& multicastTargetListActive() { return active ? multicastTargetList0 : multicastTargetList1; }
		std::vector<std::vector<entity>>& multicastTargetListInactive() { return active ? multicastTargetList1 : multicastTargetList0; }

		ComponentPool<std::vector<MessageBase*>> messageList;


	public:
		MessageManager& addUnicastMessage(std::unique_ptr<MessageBase>&& msg, entity targetId)
		{
			std::lock_guard lock(mutex);
			auto& l = unicastInactive();
			l.push_back(std::move(msg));
			unicastTargetListInactive().push_back(targetId);
			return *this;
		}
		MessageManager& addMulticastMessage(std::unique_ptr<MessageBase>&& msg, const std::vector<entity>& targetIds)
		{
			std::lock_guard lock(mutex);
			auto& l = multicastInactive();
			l.push_back(std::move(msg));
			multicastTargetListInactive().push_back(targetIds);
			return *this;
		}
		MessageManager& addBroadcastMessage(std::unique_ptr<MessageBase>&& msg)
		{
			std::lock_guard lock(mutex);
			auto& l = broadcastInactive();
			l.push_back(std::move(msg));
			return *this;
		}

		std::vector<MessageBase*>* getMessageList(entity id)
		{
			std::lock_guard lock(mutex);
			return messageList.get(id);
		}

		MessageTypeManager& getMessageTypeManager() { return messageTypeManager; };

		void subscribe(entity id)//订阅消息，任何未订阅消息的实体不会收到任何消息，订阅后将接收广播消息，作为目标时接收组播/单播消息
		{
			//std::lock_guard lock(mutex);自身有锁
			messageList.add(id, std::vector<MessageBase*>());
		}
		void unsubscribe(entity id)//取消订阅，将不会收到任何消息
		{
			//std::lock_guard lock(mutex);自身有锁
			messageList.remove(id);
		}

		void sendAll()//发送所有消息到目标消息列表
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

		void sendAll(ThreadPool& tp)//发送所有消息到目标消息列表
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

		void swap()//清除活跃消息及已发送消息队列，并切换活跃区
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
	};

	using MessageList = ComponentPool<std::vector<MessageBase*>>;
}