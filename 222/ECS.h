#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <typeindex>
#include <execution>
#include <algorithm>
#include <array>

#include <raylib.h>
#include <raymath.h>

#include "attachedLib.h"

namespace ecs
{
	template<typename T>
	class DoubleBuffered;

	template<typename T>
	class ComponentPool;

	template<typename T>
	using DoubleComs = ecs::DoubleBuffered<ecs::ComponentPool<T>*>;

	using entity = uint64_t;

	class IdManager
	{
	private:
		uint64_t countId = 0;
		std::vector<entity> inactive;
	public:
		uint64_t getId()
		{
			if (inactive.empty())
			{
				return countId++;
			}
			else
			{
				uint64_t tmp = inactive.back();
				inactive.pop_back();
				return tmp;
			}
		}
		void recycleId(uint64_t id)
		{
			inactive.push_back(id);
		}
		size_t size() const
		{
			return countId - inactive.size();
		}
	};

	class ComponentPoolBase
	{
	public:
		ComponentPoolBase() = default;
		virtual ComponentPoolBase& remove(const entity id) = 0;
		virtual ~ComponentPoolBase() = default;
	};

	//using T = int;
	template<typename T>
	class ComponentPool : public ComponentPoolBase
	{
	private:
		std::vector<T> dataVector;
		std::vector<entity> indexToEntity;
		std::vector<size_t> entityToIndex;
		mutable std::mutex mtx;

	public:
		ComponentPool() = default;

		ComponentPool& add(const entity id, const T& dat)
		{
			std::lock_guard<std::mutex> lockMtx(mtx);
			if (id < entityToIndex.size() && entityToIndex[id] != SIZE_MAX)
			{
				dataVector[entityToIndex[id]] = dat;
				return *this;
			}
			indexToEntity.push_back(id);
			dataVector.push_back(dat);
			if (id >= entityToIndex.size())
			{
				entityToIndex.resize(id + 20, SIZE_MAX);
			}
			entityToIndex[id] = dataVector.size() - 1;
			return *this;
		}
		ComponentPool& add(const entity id, T&& dat)
		{
			std::lock_guard<std::mutex> lockMtx(mtx);
			if (id < entityToIndex.size() && entityToIndex[id] != SIZE_MAX)
			{
				dataVector[entityToIndex[id]] = std::move(dat);
				return *this;
			}
			indexToEntity.push_back(id);
			dataVector.push_back(std::move(dat));
			if (id >= entityToIndex.size())
			{
				entityToIndex.resize(id + 20, SIZE_MAX);
			}
			entityToIndex[id] = dataVector.size() - 1;
			return *this;
		}
		ComponentPool& remove(const entity id) override
		{
			std::lock_guard<std::mutex> lockMtx(mtx);
			if (id >= entityToIndex.size())
			{
				return *this;
			}
			if (entityToIndex[id] == SIZE_MAX)
			{
				return *this;
			}
			else
			{
				const size_t idx = entityToIndex[id];
				const size_t lastIdx = dataVector.size() - 1;

				if (idx != dataVector.size() - 1)
				{
					entityToIndex[indexToEntity[lastIdx]] = idx;
					dataVector[idx] = std::move(dataVector[lastIdx]);
					indexToEntity[idx] = indexToEntity[lastIdx];
				}

				dataVector.pop_back();
				indexToEntity.pop_back();
				entityToIndex[id] = SIZE_MAX;

				return *this;
			}
		}

		//call func(id, Compornent, args...)
		template<typename Func, typename ...Args>
		void forEach(Func&& func, Args&& ...args)
		{
			for (size_t i = 0; i < dataVector.size(); i++)
			{
				func(indexToEntity[i], dataVector[i], std::forward<Args>(args)...);
			}
		}
		template<typename Func, typename ...Args>
		void forEach(Func&& func, Args&& ...args) const
		{
			for (size_t i = 0; i < dataVector.size(); i++)
			{
				func(indexToEntity[i], dataVector[i], std::forward<Args>(args)...);
			}
		}

		template<typename Func, typename ...Args>
		void forEach(ThreadPool& tp, Func&& func, Args&& ...args)
		{
			for (size_t i = 0; i < dataVector.size(); i++)
			{
				tp.enqueue(std::forward<Func>(func), indexToEntity[i], dataVector[i], std::forward<Args>(args)...);
			}
			tp.wait();
		}
		template<typename Func, typename ...Args>
		void forEach(ThreadPool& tp, Func&& func, Args&& ...args) const
		{
			for (size_t i = 0; i < dataVector.size(); i++)
			{
				tp.enqueue(std::forward<Func>(func), indexToEntity[i], dataVector[i], std::forward<Args>(args)...);
			}
			tp.wait();
		}

		T* get(const entity id)
		{
			std::lock_guard<std::mutex> lockMtx(mtx);
			if (id >= entityToIndex.size())
			{
				return nullptr;
			}
			if (entityToIndex[id] == SIZE_MAX)
			{
				return nullptr;
			}
			else
			{
				return &dataVector[entityToIndex[id]];
			}
		}
		const T* get(const entity id) const
		{
			std::lock_guard<std::mutex> lockMtx(mtx);
			if (id >= entityToIndex.size())
			{
				return nullptr;
			}
			if (entityToIndex[id] == SIZE_MAX)
			{
				return nullptr;
			}
			else
			{
				return &dataVector[entityToIndex[id]];
			}
		}
		size_t size() const
		{
			std::lock_guard<std::mutex> lockMtx(mtx);
			return (dataVector.size());
		}
		//必须接收锁以维持
		std::unique_lock<std::mutex> getLock()
		{
			return std::unique_lock<std::mutex>(mtx);
		}
		void reserve(size_t size)
		{
			std::lock_guard<std::mutex> lockMtx(mtx);
			dataVector.reserve(size);
			indexToEntity.reserve(size);
		}
		void clear()
		{
			std::lock_guard<std::mutex> lockMtx(mtx);
			dataVector.clear();
			indexToEntity.clear();
			entityToIndex.clear();
		}
		void shrink_to_fit()
		{
			std::lock_guard<std::mutex> lockMtx(mtx);
			dataVector.shrink_to_fit();
			indexToEntity.shrink_to_fit();
			while (entityToIndex.back() == SIZE_MAX && !entityToIndex.empty())
			{
				entityToIndex.pop_back();
			}
			entityToIndex.shrink_to_fit();
		}
	};

	class SystemBase
	{
	public:
		SystemBase() = default;
		virtual ~SystemBase() = default;
		virtual void update() = 0;
	};

	class DrawBase
	{
	public:
		DrawBase() = default;
		virtual ~DrawBase() = default;
		virtual void draw() = 0;
	};


	class AABB
	{
	private:
		Vector2 centralPoint;
		float xHarfLength;
		float yHarfLength;

	public:
		AABB() = default;
		AABB(Vector2 central, float harfX, float harfY) : centralPoint(central), xHarfLength(harfX), yHarfLength(harfY) {}
		AABB(Vector2 central, float radius) : centralPoint(central), xHarfLength(radius), yHarfLength(radius) {}

		const Vector2 getPosition() const
		{
			return centralPoint;
		}
		Vector2 getPosition()
		{
			return centralPoint;
		}
		const float getXHarfLength() const
		{
			return xHarfLength;
		}
		float getXHarfLength()
		{
			return xHarfLength;
		}
		const float getYHarfLength() const
		{
			return yHarfLength;
		}
		float getYHarfLength()
		{
			return yHarfLength;
		}
		bool isCross(const AABB& other) const
		{
			if (
				other.centralPoint.x - other.xHarfLength > centralPoint.x + xHarfLength ||
				other.centralPoint.x + other.xHarfLength < centralPoint.x - xHarfLength ||
				other.centralPoint.y - other.yHarfLength > centralPoint.y + yHarfLength ||
				other.centralPoint.y + other.yHarfLength < centralPoint.y - yHarfLength
				)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		bool isIncluding(const AABB& other) const
		{
			if (
				other.centralPoint.x - other.xHarfLength < centralPoint.x - xHarfLength ||
				other.centralPoint.x + other.xHarfLength > centralPoint.x + xHarfLength ||
				other.centralPoint.y - other.yHarfLength < centralPoint.y - yHarfLength ||
				other.centralPoint.y + other.yHarfLength > centralPoint.y + yHarfLength
				)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		bool isIncluding(const Vector2 point) const
		{
			if (
				point.x < centralPoint.x - xHarfLength ||
				point.x > centralPoint.x + xHarfLength ||
				point.y < centralPoint.y - yHarfLength ||
				point.y > centralPoint.y + yHarfLength
				)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	};

	class QuadTree
	{
	private:
		AABB selfAABB;

		static constexpr uint8_t MAX_LEVEL = 5;
		static constexpr uint8_t MAX_OBJECTS = 5;

		std::vector<entity> entityList;
		std::vector<AABB> aabbList;

		uint8_t selfLevel = 0;

		std::unique_ptr<QuadTree> NE;
		std::unique_ptr<QuadTree> SE;
		std::unique_ptr<QuadTree> NW;
		std::unique_ptr<QuadTree> SW;

	public:
		QuadTree() = default;
		QuadTree(AABB aabb, std::vector<entity> entitys, std::vector<AABB> aabbsOfEntity, uint8_t level) : selfAABB(aabb), entityList(entitys), aabbList(aabbsOfEntity), selfLevel(level)
		{
			if (entitys.size() > MAX_OBJECTS && level < MAX_LEVEL)
			{
				split();
			}
		}

		void split()
		{
			float dx = selfAABB.getXHarfLength() * 0.5f;
			float dy = selfAABB.getYHarfLength() * 0.5f;
			std::vector<entity> argEntityList[4];
			std::vector<AABB> argAABBList[4];

			AABB aabbs[4] =
			{
				{
					{
					selfAABB.getPosition().x + dx,
					selfAABB.getPosition().y + dy
					},dx,dy
				},
				{
					{
					selfAABB.getPosition().x - dx,
					selfAABB.getPosition().y + dy
					},dx,dy
				},
				{
					{
					selfAABB.getPosition().x - dx,
					selfAABB.getPosition().y - dy
					},dx,dy
				},
				{
					{
					selfAABB.getPosition().x + dx,
					selfAABB.getPosition().y - dy
					},dx,dy
				}
			};
			for (int i = 0; i < entityList.size();)
			{
				if (aabbs[0].isIncluding(aabbList[i]))
				{
					argAABBList[0].push_back(aabbList[i]);
					argEntityList[0].push_back(entityList[i]);
					aabbList.erase(aabbList.begin() + i);
					entityList.erase(entityList.begin() + i);
				}
				else if (aabbs[1].isIncluding(aabbList[i]))
				{
					argAABBList[1].push_back(aabbList[i]);
					argEntityList[1].push_back(entityList[i]);
					aabbList.erase(aabbList.begin() + i);
					entityList.erase(entityList.begin() + i);
				}
				else if (aabbs[2].isIncluding(aabbList[i]))
				{
					argAABBList[2].push_back(aabbList[i]);
					argEntityList[2].push_back(entityList[i]);
					aabbList.erase(aabbList.begin() + i);
					entityList.erase(entityList.begin() + i);
				}
				else if (aabbs[3].isIncluding(aabbList[i]))
				{
					argAABBList[3].push_back(aabbList[i]);
					argEntityList[3].push_back(entityList[i]);
					aabbList.erase(aabbList.begin() + i);
					entityList.erase(entityList.begin() + i);
				}
				else
				{
					i++;
				}
			}
			NE = std::make_unique<QuadTree>(aabbs[0], std::move(argEntityList[0]), std::move(argAABBList[0]), selfLevel + 1);
			NW = std::make_unique<QuadTree>(aabbs[1], std::move(argEntityList[1]), std::move(argAABBList[1]), selfLevel + 1);
			SW = std::make_unique<QuadTree>(aabbs[2], std::move(argEntityList[2]), std::move(argAABBList[2]), selfLevel + 1);
			SE = std::make_unique<QuadTree>(aabbs[3], std::move(argEntityList[3]), std::move(argAABBList[3]), selfLevel + 1);
		}
		void query(const AABB& targetAABB, std::vector<entity>& result)
		{
			if (!selfAABB.isCross(targetAABB))
			{
				return;
			}
			else
			{
				for (auto e : entityList)
				{
					result.push_back(e);
				}
				if (NE)
				{
					NE->query(targetAABB, result);
					NW->query(targetAABB, result);
					SW->query(targetAABB, result);
					SE->query(targetAABB, result);
				}
			}
		}
		void query(entity id, std::vector<entity>& result)
		{
			for (size_t i = 0; i < entityList.size(); ++i)
			{
				if (entityList[i] == id)
				{
					query(aabbList[i], result);
					return;
				}
			}
			if (NE)
			{
				NE->query(id, result);
				NW->query(id, result);
				SW->query(id, result);
				SE->query(id, result);
			}
		}
		void add(entity id, AABB aabb)
		{
			if (NE)
			{
				if (NE->selfAABB.isIncluding(aabb))
				{
					NE->add(id, aabb);
				}
				else if (NW->selfAABB.isIncluding(aabb))
				{
					NW->add(id, aabb);
				}
				else if (SW->selfAABB.isIncluding(aabb))
				{
					SW->add(id, aabb);
				}
				else if (SE->selfAABB.isIncluding(aabb))
				{
					SE->add(id, aabb);
				}
				else
				{
					entityList.push_back(id);
					aabbList.push_back(aabb);
				}
			}
			else
			{
				entityList.push_back(id);
				aabbList.push_back(aabb);
				if (entityList.size() > MAX_OBJECTS && selfLevel < MAX_LEVEL)
				{
					split();
				}
			}
		}
		void destroy()
		{
			entityList.clear();
			aabbList.clear();
			NE.reset();
			NW.reset();
			SW.reset();
			SE.reset();
		}
	};

	class DoubleBufferedBase
	{
	public:
		virtual void swap() {}
		DoubleBufferedBase() = default;
		virtual ~DoubleBufferedBase() = default;
	};

	template<typename T>
	class DoubleBuffered : public DoubleBufferedBase
	{
	private:
		T data0;
		T data1;
		bool state;
	public:
		DoubleBuffered(T buffer0, T buffer1, bool state) : data0(buffer0), data1(buffer1), state(state) {}
		const T& active() const
		{
			return state ? data0 : data1;
		}
		T& active()
		{
			return state ? data0 : data1;
		}
		const T& inactive() const
		{
			return state ? data1 : data0;
		}
		T& inactive()
		{
			return state ? data1 : data0;
		}
		void swap()
		{
			state = !state;
		}
	};

	//using T = int;
}