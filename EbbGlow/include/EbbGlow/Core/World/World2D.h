#pragma once

#include <unordered_map>
#include <memory>

#include <EbbGlow/Core/World/WorldType.h>
#include <EbbGlow/Resources/Resources.h>
#include <EbbGlow/Utils/Types.h>

namespace ebbglow::core
{
	class World2D
	{
	private:
		IdManager entityManager;
		std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> comPools0;
		std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> comPools1;
		std::unordered_map<std::type_index, std::unique_ptr<DoubleBufferedBase>> doubleBuffered;

		std::vector<SystemItem> systems;
		Layer unitsLayer;
		Layer uiLayer;
		MessageManager messageManager;
		Camera2D camera;

		std::vector<entity> waitDelete;
		std::unordered_map<std::type_index, std::unique_ptr<AddBufferBase>> waitAdd;

		utils::ThreadPool* threadPool = nullptr;

		template<typename T>
		ComponentPool<T>* getPool0()
		{
			auto it = comPools0.find(std::type_index(typeid(T)));
			if (it != comPools0.end())
			{
				return static_cast<ComponentPool<T>*>((it->second).get());
			}
			else
			{
				return nullptr;
			}
		}
		template<typename T>
		ComponentPool<T>* getPool1()
		{
			auto it = comPools1.find(std::type_index(typeid(T)));
			if (it != comPools1.end())
			{
				return static_cast<ComponentPool<T>*>((it->second).get());
			}
			else
			{
				return nullptr;
			}
		}
	public:
		uint64_t framesCount = 0;

		IdManager* getEntityManager() { return &entityManager; }
		Layer* getUnitsLayer() { return &unitsLayer; }
		Layer* getUiLayer() { return &uiLayer; }
		Camera2D* getCamera() { return &camera; }
		MessageManager* getMessageManager() { return &messageManager; }

		World2D() = default;
		World2D& setThreadPool(utils::ThreadPool* tp)
		{
			threadPool = tp;
			return *this;
		}

		utils::ThreadPool* getThreadPool() { return threadPool; }

		template<typename T>
		World2D& addPool()
		{
			ComponentPool<T>* pool0 = new ComponentPool<T>();
			ComponentPool<T>* pool1 = new ComponentPool<T>();
			comPools0.emplace(std::type_index(typeid(T)), std::unique_ptr<ComponentPoolBase>(pool0));
			comPools1.emplace(std::type_index(typeid(T)), std::unique_ptr<ComponentPoolBase>(pool1));
			doubleBuffered.emplace(std::type_index(typeid(T)), std::unique_ptr<DoubleBufferedBase>(new DoubleBuffered<ComponentPool<T>*>(getPool0<T>(), getPool1<T>(), framesCount % 2 == 0)));
			waitAdd.emplace(std::type_index(typeid(T)), std::unique_ptr<AddBufferBase>(new AddBuffer<T>(pool0, pool1)));
			return *this;
		}
		template<typename T>
		DoubleBuffered<ComponentPool<T>*>* getDoubleBuffer()
		{
			auto it = doubleBuffered.find(std::type_index(typeid(T)));
			if (it == doubleBuffered.end())
			{
				return nullptr;
			}
			else
			{
				return static_cast<DoubleBuffered<ComponentPool<T>*>*>(it->second.get());
			}
		}
		template<typename T>
		World2D& addSystem(T&& sys, SystemPriorityT priority = SystemPriority::Normal, SystemExecutionPolicies exePolicies = SystemExecutionPolicies::Sequential)
		{
			if (getSystem<T>()) return *this;
			auto sysPtr = std::make_unique<std::decay_t<T>>(std::forward<T>(sys));
			systems.resize(systems.size() + 1);
			auto it = systems.end() - 1;
			auto begin = systems.begin();
			while (it != begin)
			{
				if ((it - 1)->priority <= priority) break;
				*it = std::move(*(it - 1));
				--it;
			}
			*it = SystemItem{ std::move(sysPtr), priority, exePolicies };
			return *this;
		}
		template<typename T>
		T* getSystem()
		{
			for (auto& s : systems)
			{
				if (typeid(*s.system) == typeid(T)) return static_cast<T*>(s.system.get());
			}
			return nullptr;
		}

		template<typename T, typename...Args>
		World2D& createUnit(entity id, T com, Args...args)
		{
			auto it = waitAdd.find(std::type_index(typeid(T)));
			if (it != waitAdd.end())
			{
				AddBuffer<T>* buffer = static_cast<AddBuffer<T>*>((it->second).get());
				buffer->addBuffer(id, std::forward<T>(com));
			}
			createUnit(id, std::forward<Args>(args)...);
			return *this;
		}
		inline void createUnit(entity id) noexcept {}
		void deleteUnit(entity id)
		{
			waitDelete.push_back(id);
			for (auto& buffer : waitAdd)
			{
				buffer.second->deleteUnit(id);//从添加缓冲区删除，处理一帧内被创建又被删除的实体
			}
		}

		template<typename T>
		T* getWaitAdd(entity id)
		{
			auto it = waitAdd.find(std::type_index(typeid(T)));
			if (it == waitAdd.end())
			{
				return nullptr;
			}
			else
			{
				AddBuffer<T>* buffer = static_cast<AddBuffer<T>*>((it->second).get());
				for (auto& [eid, com] : buffer->buffer)
				{
					if (eid == id)
					{
						return &com;
					}
				}
				return nullptr;
			}
		}

		void draw();
		void update();
	};
}