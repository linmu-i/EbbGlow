#pragma once

#include <array>

#include <EbbGlow/Core/ECS.h>
#include <EbbGlow/Core/Message.h>

namespace ebbglow::core
{
	namespace DrawPriority
	{
		using Type = uint16_t;
	};
	
	using DrawPriorityT = DrawPriority::Type;

	struct LayerItem
	{
		DrawBase* pkt;
		DrawPriorityT priority;
	};

	class Layer
	{
	private:
		std::vector<std::unique_ptr<DrawBase>> storageArr;
		std::vector<LayerItem> drawArr;
		std::unique_ptr<std::mutex> mtx;

	public:
		Layer() : mtx(std::make_unique<std::mutex>()) {}
		Layer& add(std::unique_ptr<DrawBase>&& package, DrawPriorityT priority)
		{
			std::lock_guard<std::mutex> lock(*mtx);
			storageArr.push_back(std::move(package));
			drawArr.push_back({storageArr.back().get(), priority});
			return *this;
		}
		Layer& addRef(DrawBase* package, DrawPriorityT priority)
		{
			std::lock_guard<std::mutex> lock(*mtx);
			drawArr.push_back({package,	priority});
			return *this;
			
		}
		void draw()
		{
			for (auto& pktRef : drawArr)
			{
				pktRef.pkt->draw();
			}
		}
		void clear() noexcept
		{
			storageArr.clear();
			drawArr.clear();
		}
		auto begin() noexcept { return drawArr.begin(); }
		auto end() noexcept { return drawArr.end(); }
		void sort() noexcept
		{
			std::sort(drawArr.begin(), drawArr.end(), [](const LayerItem& a, const LayerItem& b) { return a.priority < b.priority; });
		}
	};

	/*
	class Layers
	{
	private:
		std::vector<Layer> layers;

	public:
		Layers() : layers(16) {}
		Layers(size_t layerCount) : layers(layerCount) {}
		Layer& operator[](size_t idx) { return layers[idx]; }
		const Layer& operator[](size_t idx) const { return layers[idx]; }

		auto begin() noexcept { return layers.begin(); }
		auto end() noexcept { return layers.end(); }


		size_t size() const noexcept { return layers.size(); }

		void clear() noexcept
		{
			for (auto& layer : layers)
			{
				layer.clear();
			}
		}
	};
	*/

	class AddBufferBase
	{
	protected:
		ComponentPoolBase* pool0;
		ComponentPoolBase* pool1;
	public:
		AddBufferBase(ComponentPoolBase* pool0, ComponentPoolBase* pool1) : pool0(pool0), pool1(pool1) {}
		virtual ~AddBufferBase() = default;
		virtual void addToPool() = 0;
		virtual void deleteUnit(core::entity id) = 0;
	};

	template<typename T>
	class AddBuffer : public AddBufferBase
	{
	private:
		std::vector<std::pair<entity, T>> buffer;

		friend class World2D;

	public:
		AddBuffer(ComponentPoolBase* pool0, ComponentPoolBase* pool1) : AddBufferBase(pool0, pool1) {}
		template<typename U>
		void addBuffer(entity id, U&& data)
		{
			buffer.emplace_back(id, std::forward<U>(data));
		}

		void addToPool() override
		{
			ComponentPool<T>* p0 = static_cast<ComponentPool<T>*>(pool0);
			ComponentPool<T>* p1 = static_cast<ComponentPool<T>*>(pool1);
			for (auto& [id, data] : buffer)
			{
				p0->add(id, data);
				p1->add(id, std::move(data));
			}
			buffer.clear();
		}

		void deleteUnit(core::entity id) override
		{
			buffer.erase(std::remove_if(buffer.begin(), buffer.end(), [id](const std::pair<entity, T>& dat) {return dat.first == id; }), buffer.end());
		}
	};

	namespace SystemPriority
	{
		using Type = uint16_t;

		constexpr Type Highest = 0;
		constexpr Type High = 2000;
		constexpr Type Normal = 20000;
		constexpr Type Low = 40000;
		constexpr Type Lowest = 65535;
	};

	using SystemPriorityT = SystemPriority::Type;

	enum class SystemExecutionPolicies
	{
		Sequential,
		Detached,
	};

	struct SystemItem
	{
		std::unique_ptr<SystemBase> system;
		SystemPriorityT priority = 0;
		SystemExecutionPolicies executionPolicy = SystemExecutionPolicies::Sequential;
		SystemItem() = default;
		SystemItem(std::unique_ptr<SystemBase> sys, SystemPriorityT prio, SystemExecutionPolicies policy)
			: system(std::move(sys)), priority(prio), executionPolicy(policy) {
		}
	};
}