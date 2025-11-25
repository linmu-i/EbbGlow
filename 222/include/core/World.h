#pragma once

#include <core/ECS.h>
#include <core/Message.h>
#include <utils/Resource.h>

namespace ecs
{
	using Layers = std::array<std::vector<std::unique_ptr<DrawBase>>, 16>;

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
			comPools0.emplace(std::type_index(typeid(T)), std::unique_ptr<ComponentPoolBase>(new ComponentPool<T>));
			comPools1.emplace(std::type_index(typeid(T)), std::unique_ptr<ComponentPoolBase>(new ComponentPool<T>));
			doubleBuffered.emplace(std::type_index(typeid(T)), std::unique_ptr<DoubleBufferedBase>(new DoubleBuffered<ComponentPool<T>*>(getPool0<T>(), getPool1<T>(), framesCount % 2 == 0)));
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
			getPool0<std::decay_t<T>>()->add(id, com);
			getPool1<std::decay_t<T>>()->add(id, com);
			createUnit(id, args...);
			return *this;
		}
		inline void createUnit(entity id) noexcept {}
		void deleteUnit(entity id)
		{
			waitDelete.push_back(id);
		}
		void draw()
		{
			BeginTextureMode(activeRenderTexture().get());
			ClearBackground(BLACK);
			BeginMode2D(camera);
			for (int i = 0; i < 16; i++)
			{
				for (auto& d : unitsLayer[i])
				{
					d->draw();
				}
			}
			EndMode2D();
			for (int i = 0; i < 16; i++)
			{
				for (auto& d : uiLayer[i])
				{
					d->draw();
				}
			}
			for (int i = 0; i < 16; i++)
			{
				uiLayer[i].clear();
				unitsLayer[i].clear();
			}
			EndTextureMode();
			//DrawTexture(renderTexture.get().texture, 0, 0, WHITE);
			DrawTextureRec(activeRenderTexture().get().texture,
				Rectangle{0, 0, float(activeRenderTexture().get().texture.width), float(-activeRenderTexture().get().texture.height)},
				Vector2{0, 0},
				WHITE);
		}
		void update()
		{
			for (auto i : waitDelete)
			{
				for (auto& ps : comPools0)
				{
					ps.second->remove(i);
				}
				for (auto& ps : comPools1)
				{
					ps.second->remove(i);
				}
			}
			waitDelete.clear();
			messageManager.sendAll();
			for (auto& s : systems)
			{
				s->update();
			}
			for (auto& b : doubleBuffered)
			{
				b.second->swap();
			}
			messageManager.swap();
			++framesCount;
		}
	};
}