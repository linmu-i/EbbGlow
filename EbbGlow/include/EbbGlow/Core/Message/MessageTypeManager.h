#pragma once

#include <unordered_map>
#include <typeindex>

#include <EbbGlow/Core/ECS.h>
#include <EbbGlow/Core/Message/MessageType.h>

namespace ebbglow::core
{
	class MessageTypeManager
	{
	private:
		IdManager idManager;
		std::unordered_map<std::type_index, MessageTypeId> typeToId;
		std::vector<std::type_index> idToType;

	public:
		template<typename T>
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
}