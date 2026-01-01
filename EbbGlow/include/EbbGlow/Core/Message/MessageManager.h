#pragma once

#include <unordered_map>
#include <typeindex>

#include <EbbGlow/Core/ECS.h>
#include <EbbGlow/Core/Message/MessageType.h>
#include <EbbGlow/Core/Message/MessageTypeManager.h>
#include <EbbGlow/Utils/ThreadPool.h>

namespace ebbglow::core
{
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
		MessageManager& addUnicastMessage(std::unique_ptr<MessageBase>&& msg, entity targetId);
		MessageManager& addMulticastMessage(std::unique_ptr<MessageBase>&& msg, const std::vector<entity>& targetIds);
		MessageManager& addBroadcastMessage(std::unique_ptr<MessageBase>&& msg);

		std::vector<MessageBase*>* getMessageList(entity id);

		MessageTypeManager& getMessageTypeManager() { return messageTypeManager; };

		void subscribe(entity id);//订阅消息，任何未订阅消息的实体不会收到任何消息，订阅后将接收广播消息，作为目标时接收组播/单播消息
		void unsubscribe(entity id);//取消订阅，将不会收到任何消息

		void sendAll();//发送所有消息到目标消息列表

		void sendAll(utils::ThreadPool& tp);//发送所有消息到目标消息列表

		void swap();//清除活跃消息及已发送消息队列，并切换活跃区
	};
}