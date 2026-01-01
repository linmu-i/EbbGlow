#pragma once

#include <unordered_map>
#include <memory>

#include <EbbGlow/Core/World/WorldType.h>
#include <EbbGlow/Utils/Resource.h>
#include <EbbGlow/Utils/Types.h>

namespace ebbglow::core
{
	class World2D
	{
	private:
		int screenX;
		int screenY;
		IdManager entityManager;
		std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> comPools0;
		std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> comPools1;
		std::unordered_map<std::type_index, std::unique_ptr<DoubleBufferedBase>> doubleBuffered;

		std::vector<std::unique_ptr<SystemBase>> systems;
		std::array<std::vector<std::unique_ptr<DrawBase>>, 16> unitsLayer;
		std::array<std::vector<std::unique_ptr<DrawBase>>, 16> uiLayer;
		MessageManager messageManager;
		Camera2D camera;

		std::vector<entity> waitDelete;
		std::unordered_map<std::type_index, std::unique_ptr<AddBufferBase>> waitAdd;

		rsc::SharedRenderTexture2D renderTexture0;
		rsc::SharedRenderTexture2D renderTexture1;

		rsc::SharedRenderTexture2D& activeRenderTexture()
		{
			return framesCount % 2 == 0 ? renderTexture0 : renderTexture1;
		}
		rsc::SharedRenderTexture2D& inactiveRenderTexture()
		{
			return framesCount % 2 == 0 ? renderTexture1 : renderTexture0;
		}

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
		std::array<std::vector<std::unique_ptr<DrawBase>>, 16>* getUnitsLayer() { return &unitsLayer; }
		std::array<std::vector<std::unique_ptr<DrawBase>>, 16>* getUiLayer() { return &uiLayer; }
		Camera2D* getCamera() { return &camera; }
		MessageManager* getMessageManager() { return &messageManager; }

		World2D(int screenX, int screenY) : screenX(screenX), screenY(screenY), renderTexture0(screenX, screenY), renderTexture1(screenX, screenY)
		{
			camera.offset = { 0,0 };
			camera.target = { 0,0 };
			camera.rotation = 0.0f;
			camera.zoom = 1.0f;
		}

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
		World2D& addSystem(T&& sys)
		{
			systems.push_back(std::make_unique<std::decay_t<T>>(std::forward<T>(sys)));
			return *this;
		}
		template<typename T>
		T* getSystem()
		{
			for (auto& s : systems)
			{
				T* result = dynamic_cast<T*>(s.get());
				if (result != nullptr)
				{
					return result;
				}
			}
			return nullptr;
		}
		rsc::SharedRenderTexture2D& getSceenshot()
		{
			return inactiveRenderTexture();
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
		}
		void draw();
		void update();
	};
}