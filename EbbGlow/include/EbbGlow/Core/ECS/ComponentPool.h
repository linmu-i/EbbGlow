#pragma once

#include <mutex>
#include <vector>

#include <EbbGlow/Core/ECS/ECSType.h>
#include <EbbGlow/Utils/ThreadPool.h>

namespace ebbglow::core
{
	class ComponentPoolBase
	{
	public:
		ComponentPoolBase() = default;
		virtual ComponentPoolBase& remove(const entity id) = 0;
		virtual ~ComponentPoolBase() = default;
	};

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
		void forEach(utils::ThreadPool& tp, Func&& func, Args&& ...args)
		{
			for (size_t i = 0; i < dataVector.size(); i++)
			{
				tp.enqueue(std::forward<Func>(func), indexToEntity[i], dataVector[i], std::forward<Args>(args)...);
			}
			tp.wait();
		}
		template<typename Func, typename ...Args>
		void forEach(utils::ThreadPool& tp, Func&& func, Args&& ...args) const
		{
			for (size_t i = 0; i < dataVector.size(); i++)
			{
				tp.enqueue(std::forward<Func>(func), indexToEntity[i], dataVector[i], std::forward<Args>(args)...);
			}
			tp.wait();
		}

		T* get(const entity id)
		{
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
			return (dataVector.size());
		}
		bool empty() const
		{
			return dataVector.empty();
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
		void sync(const ComponentPool<T>* other)
		{
			dataVector = other->dataVector;
		}
	};
}